/*
  ------------------------------------------------------------------------------
   Project: Hardware Design with AI
   Board: ESP32-S3
   Example: Network LED Blinking on GPIO4 with Serial Monitor Debugging
   Author: Ampnics
   YouTube Channel: https://www.youtube.com/@ampnics
   GitHub Repository: https://github.com/ampnics
  ------------------------------------------------------------------------------

   Description:
   This program demonstrates how to blink an LED connected to GPIO3 of the
   ESP32-S3 while printing status messages to the Serial Monitor for debugging.

   
*/

#define LED_PIN 4  // LED connected to GPIO4 (change if needed)

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(115200);
  delay(1000); // Allow time for Serial connection to stabilize
  Serial.println("ESP32-S3 Status LED Blink Test Starting...");
  Serial.println("Project: Hardware Design with AI");
  

  // Configure LED pin as OUTPUT
  pinMode(LED_PIN, OUTPUT);
  Serial.println("LED pin initialized as OUTPUT");
}

void loop() {
  Serial.println("Status LED ON");
  digitalWrite(LED_PIN, HIGH);  // Turn LED ON
  delay(1000);

  Serial.println("Status LED OFF");
  digitalWrite(LED_PIN, LOW);   // Turn LED OFF
  delay(1000);
}
