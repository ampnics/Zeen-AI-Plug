/*
  ------------------------------------------------------------------------------
   Project: Hardware Design with AI
   Board: ESP32-S3
   Example: Relay on GPIO2 Controlled by DOWN Button on GPIO12
   Author: Ampnics
   YouTube Channel: https://www.youtube.com/@ampnics
   GitHub Repository: https://github.com/ampnics
  ------------------------------------------------------------------------------

   Description:
   This example demonstrates relay control using a DOWN button connected to GPIO12.
   The button is externally pulled-up with 10K to 3.3V.
   When pressed, the relay turns ON; when released, the relay turns OFF.
  ------------------------------------------------------------------------------
*/

#define RELAY_PIN     2     // Relay connected to GPIO2
#define DOWN_BUTTON   12    // DOWN button connected to GPIO12 (10K pull-up to 3.3V)

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32-S3 Relay Control - DOWN Button");
  Serial.println("Project: Hardware Design with AI");
  Serial.println("YouTube: https://www.youtube.com/@ampnics");
  Serial.println("GitHub : https://github.com/ampnics");
  Serial.println("-------------------------------------------");

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(DOWN_BUTTON, INPUT);

  Serial.println("Relay and DOWN Button initialized.");
}

void loop() {
  int downButtonState = digitalRead(DOWN_BUTTON);

  if (downButtonState == LOW) { // Button pressed
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("DOWN Button Pressed -> Relay ON");
  } else {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("DOWN Button Released -> Relay OFF");
  }

  delay(200);
}
