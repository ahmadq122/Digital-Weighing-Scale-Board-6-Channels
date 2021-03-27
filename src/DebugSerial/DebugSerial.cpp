#include "DebugSerial.h"

void printDebug(bool enable, const char *string)
{
    if (enable)
        Serial.println(F(string));
}

void printDebug(bool enable, String string)
{
    if (enable)
        Serial.println(string);
}

void printDebug(bool enable, long number)
{
    if (enable)
        Serial.println(String()+number);
}
