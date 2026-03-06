/*
  ------------------------------------------------------------------------------
   Project: Hardware Design with AI
   Board: ESP32-S3
   Example: Relay on GPIO2 with Serial Monitor Debugging
   Author: Ampnics
   YouTube Channel: https://www.youtube.com/@ampnics
   GitHub Repository: https://github.com/ampnics
  ------------------------------------------------------------------------------

   Description:
   This program demonstrates how to Relay is ON/OFF connected to GPIO2 of the
   ESP32-S3 while printing status messages to the Serial Monitor for debugging.

   
*/

#define Relay_PIN 2  

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(115200);
  delay(1000); // Allow time for Serial connection to stabilize
  Serial.println("ESP32-S3 Relay Test Starting...");
  Serial.println("Project: Hardware Design with AI");
  

  // Configure Relay pin as OUTPUT
  pinMode(Relay_PIN, OUTPUT);
  Serial.println("LED pin initialized as OUTPUT");
}

void loop() {
  Serial.println("Relay ON");
  digitalWrite(Relay_PIN, HIGH);  
  delay(1000);

  Serial.println("Relay OFF");
  digitalWrite(Relay_PIN, LOW);  
  delay(1000);
}
