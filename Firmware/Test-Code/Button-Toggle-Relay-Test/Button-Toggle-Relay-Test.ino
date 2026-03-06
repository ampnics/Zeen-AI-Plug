/*
  ------------------------------------------------------------------------------
   Project: Hardware Design with AI
   Board: ESP32-S3
   Example: Reliable Relay Toggle using Button on GPIO11, Relay on GPIO2, LED on GPIO4
   Author: Ampnics
   YouTube Channel: https://www.youtube.com/@ampnics
   GitHub Repository: https://github.com/ampnics
  ------------------------------------------------------------------------------

   Description:
   This program toggles a relay ON and OFF each time a push button is pressed.
   A status LED indicates the relay state (ON = LED ON).
   Button is connected with a pull-up resistor (active LOW).
  ------------------------------------------------------------------------------
*/

#define RELAY_PIN   2    // Relay connected to GPIO2
#define STATUS_LED  4    // LED connected to GPIO4
#define BUTTON_PIN  11   // Push button connected to GPIO11 (pulled-up to 3.3V)

bool relayState = false;
bool buttonPressed = false;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32-S3 Relay Toggle Control (Active LOW Button)");
  Serial.println("Project: Hardware Design with AI");
  Serial.println("YouTube: https://www.youtube.com/@ampnics");
  Serial.println("GitHub : https://github.com/ampnics");
  Serial.println("-------------------------------------------");

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // active LOW

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(STATUS_LED, LOW);
  Serial.println("System Ready. Relay & LED are OFF.");
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  // Button is pressed when LOW
  if (buttonState == LOW && !buttonPressed) {
    buttonPressed = true;  // mark as pressed
    relayState = !relayState;  // toggle relay state
    digitalWrite(RELAY_PIN, relayState);
    digitalWrite(STATUS_LED, relayState);

    if (relayState) {
      Serial.println("Button Pressed → Relay ON, LED ON");
    } else {
      Serial.println("Button Pressed → Relay OFF, LED OFF");
    }

    delay(250); // debounce and prevent double toggle
  }

  // Reset state when button released
  if (buttonState == HIGH && buttonPressed) {
    buttonPressed = false;
    delay(10); // short release delay
  }
}
