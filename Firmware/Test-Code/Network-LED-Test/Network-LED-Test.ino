/*
  ------------------------------------------------------------------------------
   Project: Hardware Design with AI
   Board: ESP32-S3
   Example: Network LED Blinking on GPIO3 with Serial Monitor Debugging
   Author: Ampnics
   YouTube Channel: https://www.youtube.com/@ampnics
   GitHub Repository: https://github.com/ampnics
  ------------------------------------------------------------------------------

   Description:
   This program demonstrates how to blink an LED connected to GPIO3 of the
   ESP32-S3 while printing status messages to the Serial Monitor for debugging.

   Note:
   - If GPIO3 is used by USB or serial communication on your ESP32-S3 module,
     change LED_PIN to another safe GPIO such as GPIO2 or GPIO4.
   - Open Serial Monitor at 115200 baud to view debug messages.
  ------------------------------------------------------------------------------
*/

#define LED_PIN 3  // LED connected to GPIO1 (change if needed)

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(115200);
  delay(1000); // Allow time for Serial connection to stabilize
  Serial.println("ESP32-S3 Network LED Blink Test Starting...");
  Serial.println("Project: Hardware Design with AI");
  

  // Configure LED pin as OUTPUT
  pinMode(LED_PIN, OUTPUT);
  Serial.println("LED pin initialized as OUTPUT");
}

void loop() {
  Serial.println("Network LED ON");
  digitalWrite(LED_PIN, HIGH);  // Turn LED ON
  delay(100);

  Serial.println("Network LED OFF");
  digitalWrite(LED_PIN, LOW);   // Turn LED OFF
  delay(100);
}
