#include <Servo.h>

Servo testServo;

const int servoPin = 9;

void setup() {
  testServo.attach(servoPin);
}

void loop() {
  for (int angle = 0; angle <= 180; angle++) {
    testServo.write(angle);
    delay(5);
  }

  for (int angle = 180; angle >= 0; angle--) {
    testServo.write(angle);
    delay(5);
  }
}
