#include <Arduino.h>
#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <GCode.h>

#include <html_template.h>

// WiFi and server setup
WiFiManager wifiManager;
AsyncWebServer server(80);

// HTML variable processor
String processor(const String &var)
{
  Position curr = getCurrentPosition();
  if (var == "X_POS")
    return String(curr.x);
  if (var == "Y_POS")
    return String(curr.y);
  if (var == "X_MIN")
    return String(MIN_X);
  if (var == "X_MAX")
    return String(MAX_X);
  if (var == "Y_MIN")
    return String(MIN_Y);
  if (var == "Y_MAX")
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

  server.on("/gcode", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              if(request->hasParam("gcode", true))
              {
                String gcode = request->getParam("gcode", true)->value();
                setGCode(gcode);
                request->send(200, "text/plain", "OK");
              }

              request->send(400, "text/plain", "Missing gcode parameter"); });

  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              Serial.println("Restarting ESP...");
              ESP.restart(); });

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request)
            {
      AsyncResponseStream *response = request->beginResponseStream("application/json");

      JsonDocument doc;

      Position pos = getCurrentPosition();
      doc["x"] = pos.x;
      doc["y"] = pos.y;
      doc["busy"] = isBusy();
      doc["raised"] = servoLift.read() == LIFT_UP_ANGLE;

      serializeJson(doc, *response);

      request->send(response); });

  server.on("/assembly", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              Serial.println("Assembly...");
              assemblyPosition();
              request->send(200, "text/plain", "OK"); });

  server.begin();
}

void loop()
{
  machineLoop();
  updateToolPosition();
}
