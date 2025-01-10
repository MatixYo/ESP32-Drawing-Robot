#include <Arduino.h>
#include <Constants.h>
#include <ESP32Servo.h>

extern Servo servoLift;
extern Servo servoLeft;
extern Servo servoRight;

Position getCurrentPosition();

void waitFor(int delayMs);

void homeXY();

void linearMove(Position &position);

void arcMove(Position center, bool clockwise = true, Position *end = nullptr);

void enableTool(bool enable = true);

bool isBusy();

void updateToolPosition();