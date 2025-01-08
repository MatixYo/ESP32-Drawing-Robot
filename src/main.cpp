#include <Arduino.h>
#include <WiFiManager.h>
#include <ESP32Servo.h>
#include <ESPAsyncWebServer.h>
#include "html_template.h" // Include the HTML template file

Servo liftServo;
Servo servoLeft;
Servo servoRight;

// Robot geometry constants
const float L1 = 25.8; // Distance between the two servos (mm)
const float L2 = 60.0; // Length of the first arm connected to servo (mm)
const float L3 = 70.0; // Length of the second arm connected to the pen (mm)

// Servo pins
static const int liftServoPin = GPIO_NUM_0;
static const int servoLeftPin = GPIO_NUM_2;
static const int servoRightPin = GPIO_NUM_1;

// Lift positions
const int liftDown = 120; // Lift down angle
const int liftUp = 172;   // Lift up angle

const float MIN_Y = 25;  // Minimum Y position
const float MAX_Y = 125; // Maximum Y position
const float MIN_X = -50; // Minimum X position
const float MAX_X = 50;  // Maximum X position

// XY position of the pen
float penX = 0;
float penY = -50; // Default position (adjust as needed)

// WiFi and server setup
WiFiManager wifiManager;
AsyncWebServer server(80);

// Function to calculate servo angles
bool calculateServoAngles(float x, float y, float &angle1, float &angle2)
{
  if (x < MIN_X || x > MAX_X || y < MIN_Y || y > MAX_Y)
  {
    return false;
  }
  // Calculate the horizontal distances from each servo
  float x1 = x + L1 / 2; // Offset for the left servo
  float x2 = x - L1 / 2; // Offset for the right servo

  // Calculate the distance from each servo base to the target point
  float D1 = sqrt(pow(x1, 2) + pow(y, 2));
  float D2 = sqrt(pow(x2, 2) + pow(y, 2));

  // Check if the point is reachable
  if (D1 > (L2 + L3) || D2 > (L2 + L3) || D1 < fabs(L2 - L3) || D2 < fabs(L2 - L3))
  {
    return false; // Target is unreachable
  }

  // Calculate the angles for the left servo (θ1)
  float gamma1 = atan2(x1, y); // Angle from the servo to the point
  float theta1 = acos((pow(L2, 2) + pow(D1, 2) - pow(L3, 2)) / (2 * L2 * D1));
  // Serial.printf("D1: %.2f, Gamma1: %.2f, Theta1: %.2f\n", D1, degrees(gamma1), degrees(theta1));
  angle1 = degrees(theta1 - gamma1); // Counterclockwise rotation for left servo

  // Calculate the angles for the right servo (θ2)
  float gamma2 = atan2(x2, y); // Angle from the servo to the point
  float theta2 = acos((pow(L2, 2) + pow(D2, 2) - pow(L3, 2)) / (2 * L2 * D2));
  // Serial.printf("D2: %.2f, Gamma2: %.2f, Theta2: %.2f\n", D2, degrees(gamma2), degrees(theta2));
  angle2 = 180 - degrees(gamma2 + theta2); // Counterclockwise rotation for right servo

  return true; // Successful calculation
}

bool movePen(float x, float y)
{
  float angle1, angle2;
  if (calculateServoAngles(x, y, angle1, angle2))
  {
    servoLeft.write(angle1);
    servoRight.write(angle2);
    penX = x;
    penY = y;
    Serial.printf("Pen moved to X: %.2f, Y: %.2f (Angles: %.2f, %.2f)\n", penX, penY, angle1, angle2);
    return true;
  }
  Serial.println("Position out of reach!");
  return false;
}

void liftPen(bool lift)
{
  liftServo.write(lift ? liftUp : liftDown);
  delay(500);
}

bool drawCircle(float x, float y, float radius)
{
  int points = 72;
  float increment = 360.0 / points;
  float travelTime = radius / points * 200;
  Serial.println("Drawing circle..." + String(travelTime) + "ms per point " + String(increment) + " degrees per point");

  liftPen(true);
  // go to first point
  if (!movePen(x + radius, y))
  {
    return false;
  }
  liftPen(false);
  // Draw a circle with the given radius centered at (x, y)
  for (float angle = 0; angle <= 360; angle += increment)
  {
    Serial.println("Angle: " + String(angle));
    float angleRad = radians(angle);
    float x1 = x + radius * cos(angleRad);
    float y1 = y + radius * sin(angleRad);
    if (!movePen(x1, y1))
    {
      return false;
    }
    delay(travelTime);
  }
  Serial.println("Circle drawn.");

  return true;
}

// HTML variable processor
String processor(const String &var)
{
  if (var == "XPOS")
    return String(penX);
  if (var == "YPOS")
    return String(penY);
  if (var == "XMIN")
    return String(MIN_X);
  if (var == "XMAX")
    return String(MAX_X);
  if (var == "YMIN")
    return String(MIN_Y);
  if (var == "YMAX")
    return String(MAX_Y);
  return String();
}

void setup()
{
  Serial.begin(115200);

  liftServo.attach(liftServoPin);
  servoLeft.attach(servoLeftPin);
  servoRight.attach(servoRightPin);

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
      penX = request->getParam("x")->value().toFloat();
      penY = request->getParam("y")->value().toFloat();

      float angle1, angle2;
      if (movePen(penX, penY)) {
        request->send(200, "text/plain", "OK");
      } else {
        request->send(400, "text/plain", "Position out of reach");
      }
    }
    request->send(400, "text/plain", "Missing params"); });

  server.on("/controlAngles", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (request->hasParam("a1") && request->hasParam("a2")) {
      float angle1 = request->getParam("a1")->value().toFloat();
      float angle2 = request->getParam("a2")->value().toFloat();
      servoLeft.write(angle1);
      servoRight.write(angle2);
      Serial.printf("Servo angles set to %.2f, %.2f\n", angle1, angle2);
      request->send(200, "text/plain", "OK");
    }
    request->send(400, "text/plain", "Missing params"); });

  server.on("/lift", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (request->hasParam("direction")) {
      String direction = request->getParam("direction")->value();
      if (direction == "up") {
        liftPen(true);
        Serial.println("Pen lifted up.");
      } else if (direction == "down") {
        liftPen(false);
        Serial.println("Pen lifted down.");
      }
    }
    request->send(200, "text/plain", "OK"); });

  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    servoRight.write(180);
    servoLeft.write(0);
    delay(500);
    Serial.println("Servos reset.");

    request->send(200, "text/plain", "OK"); });

  server.on("/circle", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    float x = request->getParam("x")->value().toFloat();
    float y = request->getParam("y")->value().toFloat();
    float radius = request->getParam("r")->value().toFloat();
    if(drawCircle(x, y, radius)) {
      request->send(200, "text/plain", "OK");
    }
    request->send(400, "text/plain", "Position out of reach"); });

  server.begin();
}

void loop()
{
  // No additional logic required for ESPAsyncWebServer
}
