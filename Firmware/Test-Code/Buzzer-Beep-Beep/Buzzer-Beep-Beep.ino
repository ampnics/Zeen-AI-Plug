/*
  -------------------------------------------------------------------------------
   Project: Gene-AI Timer Board
   Board: ESP32-S3
   Example: Buzzer Test – C Scale Melody
   Author: Ampnics
   YouTube Channel: https://www.youtube.com/@ampnics
   GitHub Repository: https://github.com/ampnics
  -------------------------------------------------------------------------------

   Description:
   Plays a 5-second C-scale melody using DDU UDU guitar-style rhythm.
   Down stroke = longer beep
   Up stroke   = shorter beep
   Active HIGH buzzer via transistor (AO3400A).
  -------------------------------------------------------------------------------
*/

#define BUZZER_PIN 17  // IO1 (connected to buzzer transistor gate)

// C scale frequencies (Hz)
int notes[] = {
  262,  // C
  294,  // D
  330,  // E
  349,  // F
  392,  // G
  440   // A
};

// Rhythm pattern: D D U | U D U
// Down = longer duration, Up = shorter duration
int durations[] = {
  700,  // D
  700,  // D
  300,  // U
  300,  // U
  700,  // D
  300   // U
};

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Buzzer C-Scale Melody Test Started...");
}

void loop() {

  for (int i = 0; i < 6; i++) {
    tone(BUZZER_PIN, notes[i]);
    delay(durations[i]);
    noTone(BUZZER_PIN);
    delay(150);  // short gap between strokes
  }

  delay(2000);  // pause before repeating melody
}
