#include <Gcode.h>

String gCode = "";

GCodeLine parseGCodeLine(const String &line)
{
    GCodeLine gcode;
    int index = 0;

    // Extract the command (e.g., G1, M3)
    while (index < line.length() && line[index] != ' ')
    {
        gcode.cmd += line[index++];
    }

    // Skip whitespace
    while (index < line.length() && line[index] == ' ')
    {
        index++;
    }

    // Parse the parameters
    while (index < line.length())
    {
        String key;
        String value;

        // Extract the key (e.g., X, Y, Z)
        while (index < line.length() && isAlpha(line[index]))
        {
            key += line[index++];
        }

        // Extract the value (e.g., 1.0, -3.5)
        while (index < line.length() && (isDigit(line[index]) || line[index] == '.' || line[index] == '-'))
        {
            value += line[index++];
        }

        if (!key.isEmpty() && !value.isEmpty())
        {
            gcode.params[key] = value.toFloat();
        }

        // Skip whitespace
        while (index < line.length() && line[index] == ' ')
        {
            index++;
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

        linearMove(pos);
    }
    else if (gline.cmd == "G28")
    {
        homeXY();
    }
    else if (gline.cmd == "M3")
    {
        enableTool();
    }
    else if (gline.cmd == "M5")
    {
        enableTool(false);
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

    const int lineEnd = gCode.indexOf('\n');

    String line;
    if (lineEnd == -1)
    {
        line = gCode;
        gCode = "";
    }
    else
    {
        line = gCode.substring(0, lineEnd);
        gCode = gCode.substring(lineEnd + 1);
    }

    GCodeLine gline = parseGCodeLine(line);
    executeLine(gline);
}
