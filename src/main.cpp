#include <Arduino.h>
#include <WiFiManager.h>
#include <ESP32Servo.h>
#include <ESPAsyncWebServer.h>
#include "html_template.h" // Include the HTML template file

Servo servo1;
Servo servo2;
Servo liftServo;

// Robot geometry constants
const float L1 = 25.8; // Distance between the two servos (mm)
const float L2 = 60.0; // Length of the first arm connected to servo (mm)
const float L3 = 70.0; // Length of the second arm connected to the pen (mm)

// Servo pins
static const int servoPin1 = GPIO_NUM_1;
static const int servoPin2 = GPIO_NUM_2;
static const int liftServoPin = GPIO_NUM_0;

// Lift positions
const int liftDown = 120; // Lift down angle
const int liftUp = 172;   // Lift up angle

// XY position of the pen
float penX = 0;
float penY = -50; // Default position (adjust as needed)

// WiFi and server setup
WiFiManager wifiManager;
AsyncWebServer server(80);

// Process variables for HTML template
String valueX = "0";   // X position (default)
String valueY = "-50"; // Y position (default)

// Function to calculate servo angles
bool calculateServoAngles(float x, float y, float &angle1, float &angle2)
{
  // Left servo position
  float x1 = -L1 / 2.0;
  float y1 = 0.0;

  // Right servo position
  float x2 = L1 / 2.0;
  float y2 = 0.0;

  // Distance from left servo to target point
  float d1 = sqrt(sq(x - x1) + sq(y - y1));

  // Distance from right servo to target point
  float d2 = sqrt(sq(x - x2) + sq(y - y2));

  // Check if the point is reachable
  if (d1 > L2 + L3 || d1 < fabs(L2 - L3) || d2 > L2 + L3 || d2 < fabs(L2 - L3))
  {
    return false; // Target is out of reach
  }

  // Inverse kinematics for left servo
  float alpha1 = atan2(y - y1, x - x1);                           // Base angle to target
  float beta1 = acos((sq(L2) + sq(d1) - sq(L3)) / (2 * L2 * d1)); // Law of cosines
  angle1 = degrees(alpha1 + beta1);                               // Combine and convert to degrees

  // Inverse kinematics for right servo
  float alpha2 = atan2(y - y2, x - x2);                           // Base angle to target
  float beta2 = acos((sq(L2) + sq(d2) - sq(L3)) / (2 * L2 * d2)); // Law of cosines
  angle2 = degrees(alpha2 - beta2);                               // Combine and convert to degrees

  // Ensure angles are within servo limits (0 to 180 degrees)
  if (angle1 < 0 || angle1 > 180 || angle2 < 0 || angle2 > 180)
  {
    return false; // Servo angles out of range
  }

  return true; // Successfully calculated angles
}

// HTML variable processor
String processor(const String &var)
{
  if (var == "XPOS")
    return valueX;
  if (var == "YPOS")
    return valueY;
  return String();
}

void setup()
{
  Serial.begin(115200);

  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  liftServo.attach(liftServoPin);

  // Initialize WiFi
  Serial.println("Connecting to WiFi...");
  wifiManager.autoConnect();

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Define route handlers
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", htmlTemplate, processor); });

  server.on("/controlXY", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (request->hasParam("x") && request->hasParam("y")) {
      valueX = request->getParam("x")->value();
      valueY = request->getParam("y")->value();
      penX = valueX.toFloat();
      penY = valueY.toFloat();

      float angle1, angle2;
      if (calculateServoAngles(penX, penY, angle1, angle2)) {
        servo1.write(angle1);
        servo2.write(angle2);  // Adjust for servo2 orientation
        Serial.printf("Pen moved to X: %.2f, Y: %.2f (Angles: %.2f, %.2f)\n", penX, penY, angle1, angle2);
      } else {
        Serial.println("Position out of reach!");
      }
    }
    request->send(200, "text/plain", "OK"); });

  server.on("/lift", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (request->hasParam("direction")) {
      String direction = request->getParam("direction")->value();
      if (direction == "up") {
        liftServo.write(liftUp);
        Serial.println("Pen lifted up.");
      } else if (direction == "down") {
        liftServo.write(liftDown);
        Serial.println("Pen lifted down.");
      }
    }
    request->send(200, "text/plain", "OK"); });

  server.begin();
}

void loop()
{
  // No additional logic required for ESPAsyncWebServer
}
