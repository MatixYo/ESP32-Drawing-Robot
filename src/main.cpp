#include <Arduino.h>
#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <Helpers.h>

#include "html_template.h"

// WiFi and server setup
WiFiManager wifiManager;
AsyncWebServer server(80);

bool drawCircle(float x, float y, float radius)
{
  int points = 36;
  float increment = 360.0 / points;
  float travelTime = radius / points * 50;
  Serial.println("Drawing circle..." + String(travelTime) + "ms per point " + String(increment) + " degrees per point");

  liftTool(true);
  // go to first point
  // movePen(x + radius, y);
  liftTool(false);
  // Draw a circle with the given radius centered at (x, y)
  for (float angle = 0; angle <= 360; angle += increment)
  {
    Serial.println("Angle: " + String(angle));
    float angleRad = radians(angle);
    float x1 = x + radius * cos(angleRad);
    float y1 = y + radius * sin(angleRad);
    // movePen(x + radius, y);
    delay(travelTime);
  }
  Serial.println("Circle drawn.");

  return true;
}

// HTML variable processor
String processor(const String &var)
{
  Position curr = getCurrentPosition();
  if (var == "XPOS")
    return String(curr.x);
  if (var == "YPOS")
    return String(curr.y);
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

  servoLift.attach(liftServoPin);
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

  Serial.println("Homing XY.");
  homeXY();

  // Define route handlers
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", htmlTemplate, processor); });

  server.on("/controlXY", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (request->hasParam("x") && request->hasParam("y")) {
      float x = request->getParam("x")->value().toFloat();
      float y = request->getParam("y")->value().toFloat();

      Position pos = {x, y};
      linearMove(pos);
      request->send(200, "text/plain", "OK");
    }
    request->send(400, "text/plain", "Missing params"); });

  server.on("/lift", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (request->hasParam("direction")) {
      String direction = request->getParam("direction")->value();
      if (direction == "up") {
        liftTool(true);
        Serial.println("Pen lifted up.");
      } else if (direction == "down") {
        liftTool(false);
        Serial.println("Pen lifted down.");
      }
    }
    request->send(200, "text/plain", "OK"); });

  server.on("/homeXY", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              homeXY();
    Serial.println("Home position set.");

    request->send(200, "text/plain", "OK"); });

  server.on("/circle", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    float radius = request->getParam("r")->value().toFloat();
    Position center = getCurrentPosition();
    center.y += radius;

    arcMove(center);
    request->send(200, "text/plain", "OK"); });

  server.begin();
}

void loop()
{
  updateToolPosition();
}
