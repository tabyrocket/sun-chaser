#include <Servo.h>
#include <EEPROM.h>

// TUNABLE PARAMETERS
const int LDR1_PIN      = A0;
const int LDR2_PIN      = A2;
const int SERVO_PIN     = 9;

const float TOLERANCE_RATIO = 0.1;
const float KP              = 0.01;

const int LOOP_INTERVAL     = 50;
const int DEBUG_INTERVAL    = 100;

const int MIN_POS       = 10;
const int MAX_POS       = 170;
const int DEFAULT_POS   = 90;

const int EEPROM_ADDR   = 0;

// NEW: EEPROM timing
const unsigned long EEPROM_DELAY = 5000; // 5 seconds

Servo horizontalServo;
int currentPos;

// Smoothed LDR values
int ldr1 = 0;
int ldr2 = 0;

// Timing
unsigned long lastLoopTime  = 0;
unsigned long lastDebugTime = 0;

// Movement tracking
unsigned long lastMoveTime = 0;
bool positionChanged = false;

void setup() {
  Serial.begin(9600);

  currentPos = EEPROM.read(EEPROM_ADDR);

  if (currentPos < MIN_POS || currentPos > MAX_POS) {
    currentPos = DEFAULT_POS;
  }

  horizontalServo.attach(SERVO_PIN);
  horizontalServo.write(currentPos);

  ldr1 = analogRead(LDR1_PIN);
  ldr2 = analogRead(LDR2_PIN);

  lastMoveTime = millis(); // initialise

  Serial.print("Sun Chaser --- Initialized at Position: ");
  Serial.println(currentPos);
}

void loop() {
  unsigned long now = millis();
  int diff = ldr1 - ldr2;
  int avg  = (ldr1 + ldr2) / 2;

  int tolerance = avg * TOLERANCE_RATIO;

  // CONTROL LOOP
  if (now - lastLoopTime >= LOOP_INTERVAL) {
    lastLoopTime = now;

    ldr1 = (ldr1 * 3 + analogRead(LDR1_PIN)) / 4;
    ldr2 = (ldr2 * 3 + analogRead(LDR2_PIN)) / 4;

    if (abs(diff) > tolerance) {
      int step = diff * KP;

      if (step == 0) {
        step = (diff > 0) ? 1 : -1;
      }

      updatePosition(step);
    }
  }

  // Delayed EEPROM write
  if (positionChanged && (now - lastMoveTime >= EEPROM_DELAY)) {
    EEPROM.update(EEPROM_ADDR, currentPos);
    positionChanged = false;
  }

  // DEBUG OUTPUT
  if (now - lastDebugTime >= DEBUG_INTERVAL) {
    lastDebugTime = now;

    float relativeDiff = 0.0;
    if (avg > 0) {
      relativeDiff = (float)abs(diff) / avg;
    }

    Serial.print("LDR1:"); Serial.print(ldr1);
    Serial.print(" LDR2:"); Serial.print(ldr2);
    Serial.print(" Diff:"); Serial.print(ldr1 - ldr2);
    Serial.print(" RelDiff:"); Serial.print(relativeDiff * 100.0, 1);
    Serial.print(" Pos:"); Serial.println(currentPos);
  }
}

// Updates position
void updatePosition(int step) {
  int targetPos = constrain(currentPos + step, MIN_POS, MAX_POS);

  if (targetPos != currentPos) {
    currentPos = targetPos;
    horizontalServo.write(currentPos);

    // Mark movement
    lastMoveTime = millis();
    positionChanged = true;
  }
}