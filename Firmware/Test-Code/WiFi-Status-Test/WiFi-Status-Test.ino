/*
  ------------------------------------------------------------------------------
   Project: Hardware Design with AI
   Board: ESP32-S3
   Example: Wi-Fi Connection with LED Blink on GPIO3
   Author: Ampnics
   YouTube Channel: https://www.youtube.com/@ampnics
   GitHub Repository: https://github.com/ampnics
  ------------------------------------------------------------------------------

   Description:
   This program connects the ESP32-S3 to a Wi-Fi network using provided SSID and
   password credentials. Once connected, it prints the assigned IP address to
   the Serial Monitor and continuously blinks an LED connected to GPIO3 every
   0.5 seconds to indicate a successful Wi-Fi connection.
  ------------------------------------------------------------------------------
*/

#include <WiFi.h>

// ====== Wi-Fi Credentials ======
const char* ssid = "601A";          // Replace with your Wi-Fi SSID
const char* password = "9633417971"; // Replace with your Wi-Fi Password

#define LED_PIN 3  // LED connected to GPIO3

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32-S3 Wi-Fi Connection Test Starting...");
  Serial.println("Project: Hardware Design with AI");
  Serial.println("YouTube: https://www.youtube.com/@ampnics");
  Serial.println("GitHub : https://github.com/ampnics");
  Serial.println("-------------------------------------------");

  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // LED OFF initially

  // Start Wi-Fi connection
  Serial.printf("Connecting to Wi-Fi: %s\n", ssid);
  WiFi.begin(ssid, password);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Once connected
  Serial.println("\nWi-Fi Connected Successfully!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // If Wi-Fi is connected, blink LED every 0.5s
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  } else {
    // If Wi-Fi disconnected, keep LED OFF and print status
    digitalWrite(LED_PIN, LOW);
    Serial.println("Wi-Fi Disconnected!");
    delay(1000);
  }
}
