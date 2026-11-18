#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <math.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// --- Pin Definitions ---
const int PIN_LED_GREEN     = 6;  // GP6
const int PIN_LED_YELLOW    = 7;  // GP7
const int PIN_LED_RED       = 8;  // GP8
const int PIN_BUZZER_ACTIVE = 10; // GP10 (Connected to PN2222 Base via 470-ohm resistor)

// --- Trigger Notification ---
int lastSentAlertLevel = -1;

// --- Earthquake Magnitude Thresholds (m/s^2) ---
const float THRESHOLD_MODERATE = 1.00; // Triggers slow Yellow beep
const float THRESHOLD_SEVERE   = 3.80; // Triggers fast Red alarm

// --- Timing Constants ---
const unsigned long SAMPLE_INTERVAL_US = 10000; // 100.0 Hz sensor loop
const unsigned long YELLOW_PERIOD_MS   = 350;   // Slow cadence: 350ms ON / 350ms OFF
const unsigned long RED_PERIOD_MS      = 100;   // Fast cadence: 100ms ON / 100ms OFF
const unsigned long ALERT_HOLD_MS      = 1500;  // Keep alert active for 1.5s after shaking stops

unsigned long lastSampleTimeUs   = 0;
unsigned long lastBlinkTimeMs    = 0;
unsigned long lastAlertTriggerMs = 0;

int currentAlertLevel = 0; // 0 = Safe, 1 = Moderate (Yellow), 2 = Severe (Red)
bool blinkState = false;

uint8_t activeAddress = 0x53;
bool usingDirectI2C = false;
float offsetX = 0.0, offsetY = 0.0, offsetZ = 0.0;

// Low-pass filter to ignore high-frequency buzzer vibrations on the breadboard
float filtX = 0.0, filtY = 0.0, filtZ = 0.0;
const float ALPHA = 0.25;

void writeRegister(uint8_t addr, uint8_t reg, uint8_t value) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

bool initSensor() {
  if (accel.begin(0x53)) { activeAddress = 0x53; usingDirectI2C = false; return true; }
  if (accel.begin(0x1D)) { activeAddress = 0x1D; usingDirectI2C = false; return true; }

  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0 && (addr == 0x53 || addr == 0x1D)) {
      activeAddress = addr;
      usingDirectI2C = true;
      writeRegister(activeAddress, 0x31, 0x09); // Full-res, +/- 4g
      writeRegister(activeAddress, 0x2C, 0x0A); // 100 Hz
      writeRegister(activeAddress, 0x2D, 0x08); // Measurement mode
      return true;
    }
  }
  return false;
}

bool readAcceleration(float &ax, float &ay, float &az) {
  if (!usingDirectI2C) {
    sensors_event_t event;
    accel.getEvent(&event);
    ax = event.acceleration.x;
    ay = event.acceleration.y;
    az = event.acceleration.z;
    return true;
  } else {
    Wire.beginTransmission(activeAddress);
    Wire.write(0x32);
    if (Wire.endTransmission(false) != 0) return false;
    if (Wire.requestFrom(activeAddress, (uint8_t)6) != 6) return false;
    int16_t rawX = Wire.read() | (Wire.read() << 8);
    int16_t rawY = Wire.read() | (Wire.read() << 8);
    int16_t rawZ = Wire.read() | (Wire.read() << 8);
    const float SCALE = 0.0039 * 9.80665;
    ax = rawX * SCALE;
    ay = rawY * SCALE;
    az = rawZ * SCALE;
    return true;
  }
}

void calibrateBaseline() {
  digitalWrite(PIN_LED_GREEN, HIGH);
  digitalWrite(PIN_LED_YELLOW, HIGH);
  digitalWrite(PIN_LED_RED, HIGH);

  float sumX = 0, sumY = 0, sumZ = 0;
  int validSamples = 0;
  while (validSamples < 100) {
    float x, y, z;
    if (readAcceleration(x, y, z)) {
      sumX += x; sumY += y; sumZ += z;
      validSamples++;
    }
    delay(10);
  }
  offsetX = sumX / 100.0;
  offsetY = sumY / 100.0;
  offsetZ = sumZ / 100.0;

  digitalWrite(PIN_LED_YELLOW, LOW);
  digitalWrite(PIN_LED_RED, LOW);

  // Brief 80ms chirp on the active buzzer to confirm wiring works
  digitalWrite(PIN_BUZZER_ACTIVE, HIGH);
  delay(80);
  digitalWrite(PIN_BUZZER_ACTIVE, LOW);
}

void setAlertState(int newLevel, unsigned long nowMs) {
  if (newLevel != currentAlertLevel) {
    currentAlertLevel = newLevel;
    lastBlinkTimeMs = nowMs;
    blinkState = true; // Start ON immediately when alert triggers
  }
}

void updateAlerts(float accelMagnitude) {
  unsigned long nowMs = millis();

  int instantLevel = 0;
  if (accelMagnitude >= THRESHOLD_SEVERE) {
    instantLevel = 2;
  } else if (accelMagnitude >= THRESHOLD_MODERATE) {
    instantLevel = 1;
  }

  // Escalate immediately or hold active alert for ALERT_HOLD_MS
  if (instantLevel > currentAlertLevel) {
    lastAlertTriggerMs = nowMs;
    setAlertState(instantLevel, nowMs);
  } else if (instantLevel == currentAlertLevel && instantLevel > 0) {
    lastAlertTriggerMs = nowMs;
  } else if (nowMs - lastAlertTriggerMs >= ALERT_HOLD_MS) {
    setAlertState(instantLevel, nowMs);
  }

  // --- LEVEL 0: SAFE (Green Solid, Buzzer OFF) ---
  if (currentAlertLevel == 0) {
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_YELLOW, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_BUZZER_ACTIVE, LOW);
  } 
  // --- LEVEL 1: MODERATE (Yellow LED + Slow Buzzer Beep) ---
  else if (currentAlertLevel == 1) {
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_RED, LOW);

    if (nowMs - lastBlinkTimeMs >= YELLOW_PERIOD_MS) {
      lastBlinkTimeMs = nowMs;
      blinkState = !blinkState;
    }
    digitalWrite(PIN_LED_YELLOW, blinkState ? HIGH : LOW);
    digitalWrite(PIN_BUZZER_ACTIVE, blinkState ? HIGH : LOW);
  } 
  // --- LEVEL 2: SEVERE (Red LED + Rapid Buzzer Alarm) ---
  else if (currentAlertLevel == 2) {
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_YELLOW, LOW);

    if (nowMs - lastBlinkTimeMs >= RED_PERIOD_MS) {
      lastBlinkTimeMs = nowMs;
      blinkState = !blinkState;
    }
    digitalWrite(PIN_LED_RED, blinkState ? HIGH : LOW);
    digitalWrite(PIN_BUZZER_ACTIVE, blinkState ? HIGH : LOW);
  }

  if (currentAlertLevel != lastSentAlertLevel) {
  Serial1.print(currentAlertLevel); // Sends '0', '1', or '2' to the Arduino Uno
  lastSentAlertLevel = currentAlertLevel;
  }
}

void setup() {
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_BUZZER_ACTIVE, OUTPUT);
  digitalWrite(PIN_BUZZER_ACTIVE, LOW);

  Serial.begin(115200);
  Serial1.setTX(0); // GP0 -> Connect to Arduino Uno Pin 0 (RX)
  Serial1.setRX(1); // GP1 (unused, but required by setup)
  Serial1.begin(9600);
  unsigned long waitStart = millis();
  while (!Serial && (millis() - waitStart < 3000)) {
    delay(10);
  }

  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();
  Wire.setClock(100000);

  while (!initSensor()) {
    digitalWrite(PIN_LED_RED, !digitalRead(PIN_LED_RED));
    delay(500);
  }

  if (!usingDirectI2C) {
    accel.setRange(ADXL345_RANGE_4_G);
    accel.setDataRate(ADXL345_DATARATE_100_HZ);
  }

  calibrateBaseline();
  lastSampleTimeUs = micros();
}

void loop() {
  unsigned long currentUs = micros();

  if (currentUs - lastSampleTimeUs >= SAMPLE_INTERVAL_US) {
    lastSampleTimeUs += SAMPLE_INTERVAL_US;

    float rawAx, rawAy, rawAz;
    if (!readAcceleration(rawAx, rawAy, rawAz)) {
      return;
    }

    float ax = rawAx - offsetX;
    float ay = rawAy - offsetY;
    float az = rawAz - offsetZ;

    if (fabs(ax) > 35.0 || fabs(ay) > 35.0 || fabs(az) > 35.0) {
      return;
    }

    filtX = ALPHA * ax + (1.0 - ALPHA) * filtX;
    filtY = ALPHA * ay + (1.0 - ALPHA) * filtY;
    filtZ = ALPHA * az + (1.0 - ALPHA) * filtZ;

    float magnitude = sqrt(filtX * filtX + filtY * filtY + filtZ * filtZ);

    updateAlerts(magnitude);

    // Output CSV: time_ms, filtX, filtY, filtZ, alert_level
    Serial.print(currentUs / 1000);
    Serial.print(",");
    Serial.print(filtX, 4);
    Serial.print(",");
    Serial.print(filtY, 4);
    Serial.print(",");
    Serial.print(filtZ, 4);
    Serial.print(",");
    Serial.println(currentAlertLevel);
  }
}
