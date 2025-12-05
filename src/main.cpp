#include <Arduino.h>
#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>

// Own libraries
#include <config.h>
#include <gcode.h>

// Website
#include <web/index_gz.h>

// WiFi and server setup
AsyncWebServer server(80);

void setup()
{
  Serial.begin(115200);

  servoLift.attach(liftServoPin);
  servoLeft.attach(servoLeftPin);
  servoRight.attach(servoRightPin);

  // Initialize WiFi
  Serial.println("Connecting to WiFi...");
  WiFiManager wifiManager;
  wifiManager.setConnectRetries(5);
  wifiManager.setWiFiAutoReconnect(true);
  wifiManager.setConnectTimeout(10);
  WiFi.setSleep(WIFI_PS_NONE);
  wifiManager.autoConnect();

  Serial.println("\nWiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize mDNS with local domain
  if (MDNS.begin(LOCAL_DOMAIN)) {
    Serial.print("mDNS responder started. Access device at: http://");
    Serial.print(LOCAL_DOMAIN);
    Serial.println("/");
  } else {
    Serial.println("Error setting up mDNS responder!");
  }

  Serial.println("Homing XY.");
  homeXY();

  // Define route handlers
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    // Check if website hashes still match
    if (request->hasHeader("If-None-Match")) {
      String etag = request->header("If-None-Match");
      if (etag.equals(index_gz_sha)) {
        Serial.println("ETag found. Sending 304");
        // Respond with HTTP 304 (Not Modified) if hashes match
        request->send(304);
        return;
      }
    }

    // Send new version otherwise
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_gz, index_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    response->addHeader("ETag", index_gz_sha);

    Serial.println("No ETag found or mismatch. Sending 200");
    request->send(response); });

  server.on("/gcode", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              if(request->hasParam("gcode", true))
              {
                String gcode = request->getParam("gcode", true)->value();
                setGCode(gcode);
                request->send(200, "text/plain", "OK");
              }

              request->send(400, "text/plain", "Missing gcode parameter"); });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
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

  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request) {
      AsyncResponseStream *response = request->beginResponseStream("application/json");

      JsonDocument doc;

      Position pos = getCurrentPosition();
      doc["minX"] = MIN_X;
      doc["maxX"] = MAX_X;
      doc["minY"] = MIN_Y;
      doc["maxY"] = MAX_Y;
      doc["homeX"] = HOMING_POSITION.x;
      doc["homeY"] = HOMING_POSITION.y;
      doc["speed"] = getSpeed();
      doc["minSpeed"] = MIN_SPEED;
      doc["maxSpeed"] = MAX_SPEED;

      serializeJson(doc, *response);

      request->send(response);
  });

  server.on("/assembly", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              Serial.println("Assembly...");
              assemblyPosition();
              request->send(200, "text/plain", "OK"); });

  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              Serial.println("Restarting ESP...");
              ESP.restart(); });

  server.begin();
}

void loop()
{
  machineLoop();
  updateToolPosition();
}
