#pragma once

#include <map>

#include <helpers.h>

struct GCodeLine
{
    String cmd;                     // Command (e.g., G1, M3)
    std::map<String, float> params; // Parameters (e.g., X, Y, Z, etc.)
};

extern String gCode;

void setGCode(String newGCode);

void machineLoop();
