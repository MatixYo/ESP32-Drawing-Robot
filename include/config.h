#pragma once

#include <Arduino.h>
#include <types.h>

// Robot geometry constants
const float L1 = 25.8; // Distance between the two servos (mm)
const float L2 = 60.0; // Length of the first arm connected to servo (mm)
const float L3 = 70.0; // Length of the second arm connected to the pen (mm)

// Lift positions
const int LIFT_DOWN_ANGLE = 120; // Lift down angle
const int LIFT_UP_ANGLE = 172;   // Lift up angle
const int LIFT_WAIT = 500;       // Lift wait time

// Constraints for the pen position
const float MIN_Y = 25;  // Minimum Y position
const float MAX_Y = 125; // Maximum Y position
const float MIN_X = -50; // Minimum X position
const float MAX_X = 50;  // Maximum X position

const Position HOMING_POSITION = {x : 0, y : MIN_Y}; // Homing position

// Speed
const float MIN_SPEED = 10; // Minimum speed mm/s
const float MAX_SPEED = 300; // Maximum speed mm/s
const float DEFAULT_SPEED = 100; // Velocity of the pen mm/s

// Tolerance for comparing positions
const float MAX_DELTA = 0.1;

// Servo pins
static const int liftServoPin = GPIO_NUM_0;
static const int servoLeftPin = GPIO_NUM_2;
static const int servoRightPin = GPIO_NUM_1;
