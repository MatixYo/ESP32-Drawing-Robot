#include <Arduino.h>
#include <Constants.h>
#include <ESP32Servo.h>

extern Servo servoLift;
extern Servo servoLeft;
extern Servo servoRight;

Position getCurrentPosition();

void homeXY();

void linearMove(Position &position);

void arcMove(Position center, bool clockwise = true, Position *end = nullptr);

void liftTool(bool up);

void updateToolPosition();