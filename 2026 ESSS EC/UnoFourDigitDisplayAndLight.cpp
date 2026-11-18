#include "SevSeg.h"
#include <SoftwareSerial.h>

SevSeg sevseg;

// Listen to Raspberry Pi Pico GP0 (TX) on Arduino Pin A0 (RX)
// (A3 is dummy TX pin)
SoftwareSerial picoSerial(A0, A3);

// Overhead Latching Status LEDs
const int PIN_OVERHEAD_YELLOW = A1;
const int PIN_OVERHEAD_RED    = A2; 

// Countdown Timer Settings
const int COVER_TIME_YELLOW = 10;
const int COVER_TIME_RED    = 30; 

int maxQuakeSeverity = 0;         
int remainingSeconds = 0;         
bool timerRunning    = false;
unsigned long lastSecondTick = 0;

void updateOverheadLEDs() {
  if (maxQuakeSeverity == 0) {
    digitalWrite(PIN_OVERHEAD_YELLOW, LOW);
    digitalWrite(PIN_OVERHEAD_RED, LOW);
  } else if (maxQuakeSeverity == 1) {
    digitalWrite(PIN_OVERHEAD_YELLOW, HIGH);
    digitalWrite(PIN_OVERHEAD_RED, LOW);
  } else if (maxQuakeSeverity == 2) {
    digitalWrite(PIN_OVERHEAD_YELLOW, LOW);
    digitalWrite(PIN_OVERHEAD_RED, HIGH);
  }
}

void triggerQuakeEvent(int level) {
  if (level > maxQuakeSeverity) {
    maxQuakeSeverity = level;
    updateOverheadLEDs();
  }
  if (level == 1 && remainingSeconds < COVER_TIME_YELLOW) {
    remainingSeconds = COVER_TIME_YELLOW;
    timerRunning = true;
    lastSecondTick = millis();
  } 
  else if (level == 2) {
    remainingSeconds = COVER_TIME_RED; 
    timerRunning = true;
    lastSecondTick = millis();
  }
}

void setup() {
  Serial.begin(115200);
  picoSerial.begin(9600);

  pinMode(PIN_OVERHEAD_YELLOW, OUTPUT);
  pinMode(PIN_OVERHEAD_RED, OUTPUT);
  updateOverheadLEDs();

  // Configure 4-Digit 7-Segment Display
  byte numDigits = 4;
  byte digitPins[] = {2, 3, 4, 5};                  // D1, D2, D3, D4
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13}; // A, B, C, D, E, F, G, DP
  bool resistorsOnSegments = false;                 // Resistors are on Digit pins 2-5
  
  byte hardwareConfig = COMMON_CATHODE; 
  bool updateWithDelays = false;
  bool leadingZeros = false;
  bool disableDecPoint = false;

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
               updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90);

  // Start display showing "0"
  sevseg.setNumber(0);
  Serial.println("4-Digit Timer Ready! Type '1' (Yellow), '2' (Red), or 'r' (Reset) to test.");
}

void loop() {
  // 1. Listen for Pico trigger ('1' = Moderate, '2' = Severe)
  if (picoSerial.available() > 0) {
    char c = picoSerial.read();
    if (c == '1' || c == '2') {
      triggerQuakeEvent(c - '0');
    }
  }

  // 2. Listen for Manual Test / Reset commands from Arduino Serial Monitor
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '1' || c == '2') {
      triggerQuakeEvent(c - '0');
    } else if (c == 'r' || c == 'R') {
      // Reset building status & timer
      maxQuakeSeverity = 0;
      remainingSeconds = 0;
      timerRunning = false;
      updateOverheadLEDs();
      sevseg.setNumber(0);
      Serial.println("System Reset!");
    }
  }

  // 3. Non-blocking 1-second countdown logic
  if (timerRunning) {
    unsigned long now = millis();
    if (now - lastSecondTick >= 1000) {
      lastSecondTick += 1000;
      remainingSeconds--;

      if (remainingSeconds <= 0) {
        remainingSeconds = 0;
        timerRunning = false;

        maxQuakeSeverity = 0;
        updateOverheadLEDs();
        
        // Display "0000" when cover time is over and evacuation begins
        sevseg.setNumber(0);
      } else {
        sevseg.setNumber(remainingSeconds);
      }
    }
  }

  // 4. Must be called continuously to multiplex the 4-digit display smoothly!
  sevseg.refreshDisplay();
}
