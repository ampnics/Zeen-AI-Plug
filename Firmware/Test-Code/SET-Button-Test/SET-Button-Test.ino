/*
  ------------------------------------------------------------------------------
   Project: Hardware Design with AI
   Board: ESP32-S3
   Example: Relay on GPIO2 Controlled by SET Button on GPIO11 (with Serial Debugging)
   Author: Ampnics
   YouTube Channel: https://www.youtube.com/@ampnics
   GitHub Repository: https://github.com/ampnics
  ------------------------------------------------------------------------------

   Description:
   This program demonstrates how to control a relay connected to GPIO2 using
   a push button (SET button) connected to GPIO11 on the ESP32-S3. The button is
   pulled up externally with a 10K resistor to 3.3V, and the relay state is
   logged over the Serial Monitor for debugging.

   Hardware Setup:
   - Relay IN → GPIO2
   - Relay VCC → 3.3V or 5V (depending on relay module)
   - Relay GND → GND
   - SET Button one side → GPIO11
   - SET Button other side → GND
   - 10K resistor between GPIO11 and 3.3V (pull-up)

   Operation:
   - When the SET button is pressed (GPIO11 goes LOW), the relay turns ON.
   - When released (GPIO11 returns HIGH), the relay turns OFF.
  ------------------------------------------------------------------------------
*/

#define RELAY_PIN    2     // Relay connected to GPIO2
#define SET_BUTTON   11    // SET Button connected to GPIO11 (pulled-up to 3.3V with 10K)

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(115200);
  delay(1000); // Allow time for Serial connection to stabilize
  Serial.println("ESP32-S3 Relay Control Test Starting...");
  Serial.println("Project: Hardware Design with AI");
  Serial.println("YouTube: https://www.youtube.com/@ampnics");
  Serial.println("GitHub : https://github.com/ampnics");
  Serial.println("-------------------------------------------");

  // Configure Relay pin as OUTPUT
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Ensure relay starts OFF
  Serial.println("Relay pin initialized as OUTPUT");

  // Configure SET Button pin as INPUT (external pull-up)
  pinMode(SET_BUTTON, INPUT);
  Serial.println("SET Button pin initialized as INPUT with external pull-up");
}

void loop() {
  // Read the SET button state
  int setButtonState = digitalRead(SET_BUTTON);

  if (setButtonState == LOW) { // SET Button pressed
    digitalWrite(RELAY_PIN, HIGH); // Turn relay ON
    Serial.println("SET Button Pressed -> Relay ON");
  } else {
    digitalWrite(RELAY_PIN, LOW);  // Turn relay OFF
    Serial.println("SET Button Released -> Relay OFF");
  }

  delay(200); // Small delay for debounce and serial readability
}
