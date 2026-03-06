/* Zeen AI Plug - ESP32-S3 Timer Board
   Features:
   - Boot splash "Zeen AI Plug"
   - Mode select: ONLINE / OFFLINE (UP/DOWN + SET)
   - ONLINE: try stored WiFi, allow captive portal provisioning via WiFiManager if needed
       - SET long-press (6s) during online state = WiFi reset + "WiFi Reset" + LED blink 3x (100ms)
   - OFFLINE: Set countdown time (HH:MM:SS) using MODE (cursor), UP/DOWN, SET to start
   - OLED display (Adafruit SSD1306)
   - Button debounce & long press detection
*/

#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiManager.h> // WiFiManager by tzapu (ESP32 compatible fork)
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- CONFIG ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pins (as per user)
const int PIN_SET = 11;   // IO11
const int PIN_DOWN = 12;  // IO12
const int PIN_UP   = 13;  // IO13
const int PIN_MODE = 14;  // IO14
const int PIN_LED  = 3;   // IO3 status LED

// Timing constants
const unsigned long BOOT_MESSAGE_MS = 3000;
const unsigned long WIFI_CONN_TIMEOUT_MS = 20000; // try connecting for 20s before portal
const unsigned long DISPLAY_REFRESH_MS = 200;
const unsigned long BUTTON_DEBOUNCE_MS = 50;
const unsigned long SET_LONGPRESS_MS = 6000; // 6 seconds

// ---------- STATES ----------
enum SystemState {
  STATE_BOOT,
  STATE_MODE_SELECT,
  STATE_ONLINE_CONNECTING,
  STATE_ONLINE_PORTAL,      // captive portal active
  STATE_ONLINE_CONNECTED,
  STATE_WIFI_RESET_BLINK,
  STATE_OFFLINE_SET_TIME,
  STATE_OFFLINE_COUNTDOWN
};

SystemState currentState = STATE_BOOT;

// ---------- Button helper ----------
struct Button {
  int pin;
  bool lastState;
  bool stableState;
  unsigned long lastDebounce;
  unsigned long pressedMillis; // when pressed started (for long press)
  bool pressedHandled;         // whether long press has been handled
};

Button btnSet   = {PIN_SET, HIGH, HIGH, 0, 0, false};
Button btnUp    = {PIN_UP, HIGH, HIGH, 0, 0, false};
Button btnDown  = {PIN_DOWN, HIGH, HIGH, 0, 0, false};
Button btnMode  = {PIN_MODE, HIGH, HIGH, 0, 0, false};

// readButtons() updates stableState and tracks long press start time for SET
void readButton(Button &b) {
  bool reading = digitalRead(b.pin);
  if (reading != b.lastState) {
    b.lastDebounce = millis();
  }
  if ((millis() - b.lastDebounce) > BUTTON_DEBOUNCE_MS) {
    if (reading != b.stableState) {
      b.stableState = reading;
      // change of stable state: if pressed (active LOW), record press start
      if (b.stableState == LOW) {
        b.pressedMillis = millis();
        b.pressedHandled = false;
      } else {
        b.pressedMillis = 0;
        b.pressedHandled = false;
      }
    }
  }
  b.lastState = reading;
}

// helper queries
bool buttonIsPressed(Button &b) {
  return (b.stableState == LOW);
}
bool buttonWasJustPressed(Button &b) {
  // we detect press via lastDebounce transition; but for simplicity, call after readButton and
  // check if pressedMillis was just set within a short time window.
  // This function is called each loop iteration; to detect edges better, you can add more state.
  // For our simple use, we will detect rising edge by checking pressedMillis > 0 and < 200ms old.
  if (b.pressedMillis != 0 && (millis() - b.pressedMillis) < 200 && !b.pressedHandled) {
    // treat as short press event (the handler should set pressedHandled true)
    return true;
  }
  return false;
}

bool buttonLongPress(Button &b, unsigned long threshold) {
  if (b.pressedMillis != 0 && (millis() - b.pressedMillis >= threshold) && !b.pressedHandled) {
    b.pressedHandled = true;
    return true;
  }
  return false;
}

// ---------- Display helpers ----------
unsigned long lastDisplay = 0;

void showSplash() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(6, 16);
  display.println("Zeen AI");
  display.setTextSize(1);
  display.setCursor(6, 42);
  display.println("Plug");
  display.display();
}

void showTextCentered(const char* line1, const char* line2=nullptr) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  int16_t x1,y1;
  uint16_t w,h;
  display.getTextBounds(line1, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w)/2, 12);
  display.println(line1);
  if (line2) {
    display.getTextBounds(line2, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w)/2, 12 + 20);
    display.println(line2);
  }
  display.display();
}

void drawWiFiIcon(int x, int y) {
  // tiny WiFi icon (3 arcs)
  display.drawCircle(x,y-1,2,SSD1306_WHITE);
  display.drawCircle(x,y-1,4,SSD1306_WHITE);
  display.drawCircle(x,y-1,6,SSD1306_WHITE);
}

// ---------- Timer / countdown ----------
uint32_t remainingSeconds = 0;
unsigned long lastCountdownTick = 0;
bool countdownRunning = false;

void startCountdown(uint32_t seconds) {
  remainingSeconds = seconds;
  lastCountdownTick = millis();
  countdownRunning = true;
}

void stopCountdown() {
  countdownRunning = false;
}

void updateCountdown() {
  if (!countdownRunning) return;
  unsigned long now = millis();
  if (now - lastCountdownTick >= 1000) {
    unsigned long elapsed = (now - lastCountdownTick) / 1000;
    if (elapsed == 0) return;
    if (elapsed >= remainingSeconds) {
      remainingSeconds = 0;
      countdownRunning = false;
      // TODO: add end-of-countdown behavior (buzzer/relay) later
    } else {
      remainingSeconds -= elapsed;
      lastCountdownTick += elapsed * 1000;
    }
  }
}

String formatHMS(uint32_t sec) {
  uint32_t h = sec / 3600;
  uint32_t m = (sec % 3600) / 60;
  uint32_t s = sec % 60;
  char buf[16];
  sprintf(buf, "%02u:%02u:%02u", (unsigned)h, (unsigned)m, (unsigned)s);
  return String(buf);
}

// ---------- Mode selection UI ----------
int modeIndex = 0; // 0 = ONLINE, 1 = OFFLINE

void showModeSelect() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(6, 6);
  display.println("Select Mode");
  display.setTextSize(2);
  display.setCursor(12, 26);
  if (modeIndex == 0) display.print(">"); else display.print(" ");
  display.print("ONLINE");
  display.setCursor(12, 46);
  if (modeIndex == 1) display.print(">"); else display.print(" ");
  display.print("OFFLINE");
  display.display();
}

// ---------- Offline set-time UI ----------
int setCursor = 0; // 0=HH,1=MM,2=SS
uint32_t setHours = 0, setMinutes = 0, setSeconds = 0;

void showOfflineSetTimeScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(6, 2);
  display.println("Set Time");

  display.setTextSize(2);
  char buf[16];
  sprintf(buf, "%02u:%02u:%02u", (unsigned)setHours, (unsigned)setMinutes, (unsigned)setSeconds);
  int16_t x1,y1; uint16_t w,h;
  display.getTextBounds(buf,0,0,&x1,&y1,&w,&h);
  display.setCursor((SCREEN_WIDTH - w)/2, 18);
  display.println(buf);

  // draw cursor under selected part
  int cursorX = (SCREEN_WIDTH - w)/2;
  if (setCursor == 0) cursorX += 0;
  else if (setCursor == 1) cursorX += 28; // approximate offsets
  else cursorX += 58;
  display.fillRect(cursorX, 42, 24, 3, SSD1306_WHITE);
  display.display();
}

// ---------- Globals for WiFi handling ----------
WiFiManager wm;
bool wifiConnectedIcon = false;

// Attempt non-blocking connect using stored credentials for up to WIFI_CONN_TIMEOUT_MS.
// During this we will poll buttons allowing a 6s SET long press to reset credentials.
// Returns true if connected; false if not connected (caller may start portal).
bool tryConnectWithStoredCredsAllowReset() {
  unsigned long start = millis();
  WiFi.mode(WIFI_STA);
  WiFi.begin(); // use stored credentials if any

  while (millis() - start < WIFI_CONN_TIMEOUT_MS) {
    // allow background wifi processing
    delay(100);

    // read buttons (to allow SET long press detection)
    readButton(btnSet); readButton(btnUp); readButton(btnDown); readButton(btnMode);

    // check for SET long press to reset credentials
    if (buttonLongPress(btnSet, SET_LONGPRESS_MS)) {
      // WiFi reset sequence requested
      WiFiManager wm2;
      wm2.resetSettings();
      // show wifi reset and blink LED 3x
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(10, 20);
      display.println("WiFi Reset");
      display.display();
      for (int i=0;i<3;i++) {
        digitalWrite(PIN_LED, HIGH);
        delay(100);
        digitalWrite(PIN_LED, LOW);
        delay(100);
      }
      // after reset, abort connect
      return false;
    }

    if (WiFi.status() == WL_CONNECTED) {
      return true;
    }
  }
  // timed out
  return false;
}

// ---------- Setup & Loop ----------
unsigned long stateEntryTime = 0;

void enterState(SystemState s) {
  currentState = s;
  stateEntryTime = millis();
  // state-specific initialization
  if (s == STATE_BOOT) {
    showSplash();
  } else if (s == STATE_MODE_SELECT) {
    showModeSelect();
  } else if (s == STATE_ONLINE_CONNECTING) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(6, 18);
    display.println("Connecting to the WiFi");
    display.display();
    wifiConnectedIcon = false;
  } else if (s == STATE_OFFLINE_SET_TIME) {
    // default set values
    setHours = 0; setMinutes = 0; setSeconds = 0;
    setCursor = 0;
    // show Set Time for 3s first
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(6, 18);
    display.println("Set Time");
    display.display();
  } else if (s == STATE_OFFLINE_COUNTDOWN) {
    // convert set values to seconds and start countdown
    uint32_t total = setHours * 3600 + setMinutes * 60 + setSeconds;
    startCountdown(total);
  }
}

void setupPins() {
  pinMode(PIN_SET, INPUT_PULLUP);
  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DOWN, INPUT_PULLUP);
  pinMode(PIN_MODE, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
}

void setup() {
  Serial.begin(115200);
  setupPins();

  // Init display
  Wire.begin(); // default I2C pins
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // hang
  }
  display.clearDisplay();
  display.display();

  // Enter boot state
  enterState(STATE_BOOT);
  delay(BOOT_MESSAGE_MS);
  enterState(STATE_MODE_SELECT);
}

void loop() {
  unsigned long now = millis();

  // read buttons (for edge/press detection in many states)
  readButton(btnSet); readButton(btnUp); readButton(btnDown); readButton(btnMode);

  // Common: handle Mode selection
  if (currentState == STATE_MODE_SELECT) {
    // respond to UP/DOWN to change modeIndex, SET to confirm
    static unsigned long lastNavTime = 0;
    if (buttonIsPressed(btnUp) && (now - lastNavTime > 200)) {
      modeIndex = max(0, modeIndex - 1);
      showModeSelect();
      lastNavTime = now;
    }
    if (buttonIsPressed(btnDown) && (now - lastNavTime > 200)) {
      modeIndex = min(1, modeIndex + 1);
      showModeSelect();
      lastNavTime = now;
    }
    // SET short press to choose
    if (buttonWasJustPressed(btnSet)) {
      btnSet.pressedHandled = true;
      if (modeIndex == 0) {
        enterState(STATE_ONLINE_CONNECTING);
      } else {
        enterState(STATE_OFFLINE_SET_TIME);
      }
    }
  }
  // ---------- ONLINE CONNECTING ----------
  else if (currentState == STATE_ONLINE_CONNECTING) {
    // Attempt to connect using stored credentials with ability to long-press SET to reset.
    bool connected = tryConnectWithStoredCredsAllowReset();

    if (connected) {
      wifiConnectedIcon = true;
      enterState(STATE_ONLINE_CONNECTED);
    } else {
      // not connected, start captive portal so user can input credentials via phone
      enterState(STATE_ONLINE_PORTAL);
    }
  }

  // ---------- ONLINE PORTAL (WiFiManager AP) ----------
  else if (currentState == STATE_ONLINE_PORTAL) {
    // Blocking call to start AP and wait for credentials -> user opens captive portal on phone
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(6, 6);
    display.println("WiFi Setup Mode");
    display.setCursor(6, 26);
    display.println("Open the AP on your phone");
    display.display();

    WiFiManager wm2;
    wm2.setTimeout(180); // portal active for up to 180s (adjustable)
    bool res = wm2.autoConnect("ZeenAI_Portal");
    if (res) {
      // connected or credentials stored; WiFiManager returns once configured and connected
      wifiConnectedIcon = (WiFi.status() == WL_CONNECTED);
      enterState(STATE_ONLINE_CONNECTED);
    } else {
      // timed out or failed; go back to mode select
      enterState(STATE_MODE_SELECT);
    }
  }

  // ---------- ONLINE CONNECTED ----------
  else if (currentState == STATE_ONLINE_CONNECTED) {
    // Display UI: WiFi icon top-right, and Remaining Time 00:00:00
    if (now - lastDisplay > DISPLAY_REFRESH_MS) {
      lastDisplay = now;
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(2, 0);
      display.println("ONLINE");
      if (wifiConnectedIcon) {
        drawWiFiIcon(SCREEN_WIDTH - 10, 8);
      }
      display.setTextSize(1);
      display.setCursor(6, 18);
      display.println("Remaining Time");
      display.setTextSize(2);
      display.setCursor(6, 34);
      display.println(formatHMS(remainingSeconds));
      display.display();
    }

    // Allow long-press SET to reset credentials while connected as well
    if (buttonLongPress(btnSet, SET_LONGPRESS_MS)) {
      // perform reset
      WiFiManager wm2;
      wm2.resetSettings();
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(6, 20);
      display.println("WiFi Reset");
      display.display();
      for (int i=0;i<3;i++) {
        digitalWrite(PIN_LED, HIGH);
        delay(100);
        digitalWrite(PIN_LED, LOW);
        delay(100);
      }
      // after reset, disconnect and go back to connecting
      WiFi.disconnect(true, true);
      enterState(STATE_ONLINE_CONNECTING);
    }

    // update countdown if running
    updateCountdown();
  }

  // ---------- OFFLINE SET TIME ----------
  else if (currentState == STATE_OFFLINE_SET_TIME) {
    // After showing "Set Time" for 3s, show the set screen
    if (millis() - stateEntryTime < 3000) {
      // still in initial 3s showing "Set Time"
      // nothing else
    } else {
      // show set UI periodically
      if (now - lastDisplay > DISPLAY_REFRESH_MS) {
        lastDisplay = now;
        showOfflineSetTimeScreen();
      }

      // Navigate cursor with MODE (move between HH:MM:SS)
      static unsigned long lastNav = 0;
      if (buttonWasJustPressed(btnMode) && (now - lastNav > 150)) {
        btnMode.pressedHandled = true;
        setCursor = (setCursor + 1) % 3;
        lastNav = now;
      }

      // UP/DOWN adjust current field
      if (buttonIsPressed(btnUp) && (now - lastNav > 120)) {
        lastNav = now;
        if (setCursor == 0) {
          setHours = (setHours + 1) % 100; // cap at 99h
        } else if (setCursor == 1) {
          setMinutes = (setMinutes + 1) % 60;
        } else {
          setSeconds = (setSeconds + 1) % 60;
        }
      }
      if (buttonIsPressed(btnDown) && (now - lastNav > 120)) {
        lastNav = now;
        if (setCursor == 0) {
          setHours = (setHours == 0) ? 99 : setHours - 1;
        } else if (setCursor == 1) {
          setMinutes = (setMinutes == 0) ? 59 : setMinutes - 1;
        } else {
          setSeconds = (setSeconds == 0) ? 59 : setSeconds - 1;
        }
      }

      // SET short press to start countdown
      if (buttonWasJustPressed(btnSet)) {
        btnSet.pressedHandled = true;
        uint32_t total = setHours * 3600 + setMinutes * 60 + setSeconds;
        if (total > 0) {
          startCountdown(total);
          enterState(STATE_OFFLINE_COUNTDOWN);
        } else {
          // nothing to start - maybe beep later
        }
      }
    }
  }

  // ---------- OFFLINE COUNTDOWN ----------
  else if (currentState == STATE_OFFLINE_COUNTDOWN) {
    // update countdown and display
    updateCountdown();
    if (now - lastDisplay > DISPLAY_REFRESH_MS) {
      lastDisplay = now;
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(6, 2);
      display.println("OFFLINE");
      display.setTextSize(2);
      display.setCursor(6, 22);
      display.println(formatHMS(remainingSeconds));
      display.display();
    }

    // while counting down allow MODE to pause/stop? We'll allow SET to stop and go back to set screen
    if (buttonWasJustPressed(btnSet)) {
      btnSet.pressedHandled = true;
      stopCountdown();
      enterState(STATE_OFFLINE_SET_TIME);
    }

    if (!countdownRunning && remainingSeconds == 0) {
      // countdown finished: you can add relay/buzzer actions here
      // For now, show "Time Up" and go back to set-time after 3s
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(10, 24);
      display.println("Time Up");
      display.display();
      delay(3000);
      enterState(STATE_OFFLINE_SET_TIME);
    }
  }

  // small yield so USB serial works well
  delay(10);
}
