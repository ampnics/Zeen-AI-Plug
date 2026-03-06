/*
  ------------------------------------------------------------------------------
   Project: Hardware Design with AI
   Board: ESP32-S3
   Example: Basic Wi-Fi Connection
   Author: Ampnics
   YouTube Channel: https://www.youtube.com/@ampnics
   GitHub Repository: https://github.com/ampnics
  ------------------------------------------------------------------------------

   Description:
   This program connects the ESP32-S3 to a Wi-Fi network using provided SSID and
   password credentials. Once connected, it prints the assigned IP address to
   the Serial Monitor.
  ------------------------------------------------------------------------------
*/

#include <WiFi.h>

// ====== Wi-Fi Credentials ======
const char* ssid = "601A";        // Replace with your Wi-Fi SSID
const char* password = "9633417971"; // Replace with your Wi-Fi Password

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32-S3 Wi-Fi Connection Test Starting...");
  Serial.println("Project: Hardware Design with AI");
  Serial.println("YouTube: https://www.youtube.com/@ampnics");
  Serial.println("GitHub : https://github.com/ampnics");
  Serial.println("-------------------------------------------");

  // Start Wi-Fi
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
  // Optional: keep checking connection status
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi Disconnected!");
  }
  delay(5000); // Check every 5 seconds
}
