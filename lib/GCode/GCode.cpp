#include <Gcode.h>

String gCode = "";

GCodeLine parseGCodeLine(const String &line)
{
    GCodeLine gcode;
    char lineBuffer[line.length() + 1];
    line.toCharArray(lineBuffer, line.length() + 1);

    char* token = strtok(lineBuffer, " ");
    if (token != nullptr) {
        gcode.cmd = token;
    }

    while ((token = strtok(nullptr, " ")) != nullptr) {
        String param(token);
        if (param.length() > 1) {
            String key = param.substring(0, 1);
            String valueStr = param.substring(1);
            gcode.params[key] = valueStr.toFloat();
        }
    }

    return gcode;
}

void executeLine(GCodeLine &gline)
{
    if (gline.cmd == "G1")
    {
        Position pos;
        pos.x = gline.params["X"];
        pos.y = gline.params["Y"];

        Serial.printf("G1: Linear move to X: %.2f, Y: %.2f\n", pos.x, pos.y);
        linearMove(pos);
    }
    else if (gline.cmd == "G28")
    {
        Serial.println("G28: Homing XY");
        homeXY();
    }
    else if (gline.cmd == "M3")
    {
        Serial.println("M3: Lower tool");
        enableTool();
    }
    else if (gline.cmd == "M5")
    {
        Serial.println("M5: Raise tool");
        enableTool(false);
    }
    else if (gline.cmd == "M999") {
        Serial.println("M999: Restarting ESP...");
        ESP.restart();
    }
    else if(gline.cmd == "M203") {
        float newSpeed = gline.params["X"];
        Serial.printf("M203: Setting speed to %.2f\n", newSpeed);
        setSpeed(newSpeed);
    }
    else {
        Serial.printf("Unknown command: %s\n", gline.cmd.c_str());
    }
}

// Public methods
void setGCode(String newGCode)
{
    gCode = newGCode;
}

void machineLoop()
{
    if (isBusy() || gCode.isEmpty())
        return;

    int lineEnd = gCode.indexOf('\n');

    String line = (lineEnd == -1) ? gCode : gCode.substring(0, lineEnd);
    gCode = (lineEnd == -1) ? "" : gCode.substring(lineEnd + 1);

    line.trim();

    if (line.isEmpty())
        return;

    GCodeLine gline = parseGCodeLine(line);
    executeLine(gline);
}
