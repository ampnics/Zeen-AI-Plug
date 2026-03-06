/* Zeen AI Plug - ESP32-S3 Timer Board (Online + Offline)
   Features:
   - Splash + C-scale on startup
   - Mode select: ONLINE / OFFLINE
   - ONLINE: WiFiManager + SinricPro control + OLED feedback
   - OFFLINE: Timer countdown with relay, buzzer & OLED
   - Perfect button debounce, single press & long press
   - Relay status LED
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- PIN DEFINITIONS ----------
#define PIN_SET       11
#define PIN_DOWN      12
#define PIN_UP        13
#define PIN_MODE      14
#define BUZZER_PIN    1
#define RELAY_PIN     2
#define PIN_RELAY_LED 4
#define WIFI_LED      3

// ---------- OLED ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool oledReady = false;

// ---------- TIMING ----------
#define BOOT_MESSAGE_MS     3000
#define DISPLAY_REFRESH_MS  150
#define BUTTON_DEBOUNCE     60
#define HOLD_REPEAT_DELAY   500
#define HOLD_REPEAT_RATE    110

// ---------- STATE MACHINE ----------
enum SystemState {STATE_BOOT, STATE_MODE_SELECT, STATE_SET_TIME, STATE_COUNTDOWN, STATE_ONLINE};
SystemState currentState;

// ---------- TIME VARIABLES ----------
uint8_t cursor = 0;
uint32_t setH = 0, setM = 0, setS = 0;
uint32_t remainingSeconds = 0;
unsigned long lastCountdownTick = 0;
bool countdownRunning = false;

// ---------- BUTTON STRUCT ----------
struct Btn { int pin; bool stable = HIGH; bool last = HIGH; unsigned long lastChange = 0; unsigned long pressedAt = 0; unsigned long lastRepeat = 0; bool handled = false; };
Btn bSet = {PIN_SET}, bUp = {PIN_UP}, bDown = {PIN_DOWN}, bMode = {PIN_MODE};

// ---------- WiFi + SinricPro ----------
WiFiManager wm;
volatile bool systemRunning = false;
unsigned long lastAnimMillis = 0;
#define APP_KEY     "e9130a32-30ea-4935-84b1-90f55e0df98e"
#define APP_SECRET  "146329c0-e815-4bd2-a43a-5ed70126cb2b-4f93b63c-ca2e-46a0-8f7e-2d3c5975c8cc"
#define SWITCH_ID_1 "67c80f958d6ea96ddf98682d"

// ---------- BUZZER ----------
void beep(uint16_t f = 3000, uint16_t d = 30) { tone(BUZZER_PIN, f, d); }

void playStartupTone() { const uint32_t notes[]={262,294,330,349,392,440,494,523}; for(int i=0;i<8;i++){beep(notes[i],200);delay(50);} delay(500); }

// ---------- BUTTON PROCESSING ----------
void readBtn(Btn &b) {
  bool r = digitalRead(b.pin);
  if(r!=b.last){b.lastChange=millis();b.last=r;}
  if(millis()-b.lastChange>BUTTON_DEBOUNCE){
    if(r!=b.stable){
      b.stable=r;
      if(b.stable==LOW){b.pressedAt=millis();b.lastRepeat=millis();b.handled=false;}
      else b.handled=false;
    }
  }
}
bool singlePress(Btn &b){if(b.stable==LOW&&!b.handled&&millis()-b.pressedAt<HOLD_REPEAT_DELAY){b.handled=true;beep();return true;}return false;}
bool repeatPress(Btn &b){if(b.stable==LOW&&millis()-b.pressedAt>=HOLD_REPEAT_DELAY&&millis()-b.lastRepeat>=HOLD_REPEAT_RATE){b.lastRepeat=millis();beep();return true;}return false;}

// ---------- DISPLAY HELPERS ----------
void splash(){display.clearDisplay();display.setTextColor(SSD1306_WHITE);display.setTextSize(2);display.setCursor(10,20);display.println("Zeen AI");display.setTextSize(1);display.setCursor(10,45);display.println("Plug");display.display();}
void showMode(){
  display.clearDisplay();display.setTextSize(1);display.setCursor(10,5);display.println("Select Mode");
  display.setTextSize(2);display.setCursor(10,25);if(cursor==0)display.print("> ");else display.print("  ");display.print("ONLINE");
  display.setTextSize(2);display.setCursor(10,45);if(cursor==1)display.print("> ");else display.print("  ");display.print("OFFLINE");display.display();
}
void showSetTime(){
  char buf[16];sprintf(buf,"%02d:%02d:%02d",setH,setM,setS);
  display.clearDisplay();display.setTextSize(1);display.setCursor(10,5);display.println("Set Time");
  display.setTextSize(2);int16_t x1,y1;uint16_t w,h;display.getTextBounds(buf,0,0,&x1,&y1,&w,&h);
  display.setCursor((128-w)/2,25);display.println(buf);
  int cursorX=(128-w)/2;if(cursor==1)cursorX+=30;else if(cursor==2)cursorX+=65;
  display.fillRect(cursorX,47,22,3,SSD1306_WHITE);display.display();
}
void showCountdown(){
  char buf[16];uint32_t sec=remainingSeconds;uint32_t h=sec/3600;sec%=3600;uint32_t m=sec/60;sec%=60;
  sprintf(buf,"%02d:%02d:%02d",h,m,sec);
  display.clearDisplay();display.setTextSize(1);display.setCursor(10,5);display.println("OFFLINE");
  display.setTextSize(2);display.setCursor(10,25);display.println(buf);display.display();
}

// ---------- COUNTDOWN ----------
void startCountdown(){remainingSeconds=setH*3600+setM*60+setS;if(!remainingSeconds)return;countdownRunning=true;lastCountdownTick=millis();digitalWrite(RELAY_PIN,HIGH);digitalWrite(PIN_RELAY_LED,HIGH);}
void stopCountdown(){countdownRunning=false;digitalWrite(RELAY_PIN,LOW);digitalWrite(PIN_RELAY_LED,LOW);}
void updateCountdown(){if(!countdownRunning)return;if(millis()-lastCountdownTick>=1000){lastCountdownTick+=1000;if(remainingSeconds>0)remainingSeconds--;if(!remainingSeconds)stopCountdown();}}

// ---------- STATE CONTROL ----------
void enterState(SystemState s){
  currentState=s;
  if(s==STATE_BOOT){splash();playStartupTone();}
  else if(s==STATE_MODE_SELECT){cursor=0;showMode();}
  else if(s==STATE_SET_TIME){setH=setM=setS=0;cursor=0;showSetTime();}
  else if(s==STATE_COUNTDOWN){startCountdown();showCountdown();}
}

// ---------- ONLINE MODE ----------
bool onPowerState1(const String &deviceId,bool &state){
  systemRunning=state;
  digitalWrite(RELAY_PIN,state?HIGH:LOW);digitalWrite(PIN_RELAY_LED,state?HIGH:LOW);
  if(state){beep(660,80);beep(880,80);}else{beep(220,150);}
  return true;
}

void tryConnectOrPortal(){
  display.clearDisplay();display.setTextSize(1);display.setCursor(0,8);display.println("Connecting WiFi...");display.display();
  WiFi.mode(WIFI_STA);WiFi.begin();
  unsigned long start=millis();while(WiFi.status()!=WL_CONNECTED&&millis()-start<10000)delay(200);
  if(WiFi.status()==WL_CONNECTED){digitalWrite(WIFI_LED,HIGH);display.clearDisplay();display.setCursor(0,8);display.println("WiFi Connected");display.display();delay(900);return;}
  display.clearDisplay();display.setCursor(0,8);display.println("AP Mode");display.display();
  wm.setTimeout(300);wm.startConfigPortal("ZeenAI-Setup");
}

// ---------- SET BUTTON LONG PRESS ----------
void resetWiFiAndRestart(){wm.resetSettings();WiFi.disconnect(true,true);ESP.restart();}
void handleSetButton(){static unsigned long pressStart=0;static bool longPressed=false;int val=digitalRead(PIN_SET);if(val==LOW){if(pressStart==0)pressStart=millis();if(!longPressed&&millis()-pressStart>=5000){longPressed=true;resetWiFiAndRestart();}}else{pressStart=0;longPressed=false;}}

// ---------- SETUP ----------
void setup(){
  Serial.begin(115200);delay(50);
  pinMode(RELAY_PIN,OUTPUT);pinMode(PIN_RELAY_LED,OUTPUT);pinMode(PIN_SET,INPUT_PULLUP);
  pinMode(PIN_UP,INPUT_PULLUP);pinMode(PIN_DOWN,INPUT_PULLUP);pinMode(PIN_MODE,INPUT_PULLUP);pinMode(WIFI_LED,OUTPUT);
  digitalWrite(RELAY_PIN,LOW);digitalWrite(PIN_RELAY_LED,LOW);digitalWrite(WIFI_LED,LOW);
  if(display.begin(SSD1306_SWITCHCAPVCC,OLED_ADDR)){oledReady=true;display.clearDisplay();display.display();}
  enterState(STATE_BOOT);delay(BOOT_MESSAGE_MS);
  enterState(STATE_MODE_SELECT);
}

// ---------- LOOP ----------
void loop(){
  readBtn(bSet);readBtn(bUp);readBtn(bDown);readBtn(bMode);
  static unsigned long lastUI=0;

  // MODE SELECT
  if(currentState==STATE_MODE_SELECT){
    if(singlePress(bUp)||repeatPress(bUp)){cursor=0;showMode();}
    if(singlePress(bDown)||repeatPress(bDown)){cursor=1;showMode();}
    if(singlePress(bSet)){
      if(cursor==0){ // ONLINE
        enterState(STATE_ONLINE);tryConnectOrPortal();
        SinricProSwitch &mySwitch=SinricPro[SWITCH_ID_1];mySwitch.onPowerState(onPowerState1);
        SinricPro.begin(APP_KEY,APP_SECRET);
      }else enterState(STATE_SET_TIME); // OFFLINE
    }
  }

  // SET TIME
  else if(currentState==STATE_SET_TIME){
    if(singlePress(bMode))cursor=(cursor+1)%3;
    if(singlePress(bUp)||repeatPress(bUp)){if(cursor==0&&setH<99)setH++;if(cursor==1&&setM<59)setM++;if(cursor==2&&setS<59)setS++;}
    if(singlePress(bDown)||repeatPress(bDown)){if(cursor==0&&setH>0)setH--;if(cursor==1&&setM>0)setM--;if(cursor==2&&setS>0)setS--;}
    if(singlePress(bSet)){if(setH+setM+setS>0)enterState(STATE_COUNTDOWN);}
    if(millis()-lastUI>DISPLAY_REFRESH_MS){lastUI=millis();showSetTime();}
  }

  // COUNTDOWN
  else if(currentState==STATE_COUNTDOWN){
    updateCountdown();
    if(millis()-lastUI>DISPLAY_REFRESH_MS){lastUI=millis();showCountdown();}
    if(singlePress(bSet)){stopCountdown();enterState(STATE_SET_TIME);}
    if(!countdownRunning&&remainingSeconds==0){display.clearDisplay();display.setTextSize(2);display.setCursor(25,25);display.println("TIME UP");display.display();delay(2000);enterState(STATE_SET_TIME);}
  }

  // ONLINE RUN
  else if(currentState==STATE_ONLINE){
    SinricPro.handle();digitalWrite(WIFI_LED,WiFi.status()==WL_CONNECTED?HIGH:LOW);
    handleSetButton();
    if(systemRunning){if(oledReady&&millis()-lastAnimMillis>450){lastAnimMillis=millis();display.clearDisplay();display.setTextSize(2);display.setCursor(10,0);display.println("SYSTEM ON");display.display();}}
    else{if(oledReady&&millis()-lastAnimMillis>450){lastAnimMillis=millis();display.clearDisplay();display.setTextSize(2);display.setCursor(6,8);display.println("SYSTEM OFF");display.display();}}
  }
}
