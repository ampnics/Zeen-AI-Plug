/*
  ------------------------------------------------------------------------------
   Project: Hardware Design with AI
   Board: ESP32-S3
   Example: Relay on GPIO2 Controlled by MODE Button on GPIO14
   Author: Ampnics
   YouTube Channel: https://www.youtube.com/@ampnics
   GitHub Repository: https://github.com/ampnics
  ------------------------------------------------------------------------------

   Description:
   This example demonstrates relay control using a MODE button connected to GPIO14.
   The button is externally pulled-up with 10K to 3.3V.
   When pressed, the relay turns ON; when released, the relay turns OFF.
  ------------------------------------------------------------------------------
*/

#define RELAY_PIN     2     // Relay connected to GPIO2
#define MODE_BUTTON   14    // MODE button connected to GPIO14 (10K pull-up to 3.3V)

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32-S3 Relay Control - MODE Button");
  Serial.println("Project: Hardware Design with AI");
  Serial.println("YouTube: https://www.youtube.com/@ampnics");
  Serial.println("GitHub : https://github.com/ampnics");
  Serial.println("-------------------------------------------");

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(MODE_BUTTON, INPUT);

  Serial.println("Relay and MODE Button initialized.");
}

void loop() {
  int modeButtonState = digitalRead(MODE_BUTTON);

  if (modeButtonState == LOW) { // Button pressed
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("MODE Button Pressed -> Relay ON");
  } else {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("MODE Button Released -> Relay OFF");
  }

  delay(200);
}
