#include "DebugSerial.h"
#include "FlashMemory/FlashMemory.h"

void printDebug(const char *string)
{
    if (data.getDebugMode())
        Serial.println(F(string));
}

void printDebug(String string)
{
    if (data.getDebugMode())
        Serial.println(string);
}

void printDebug(long number)
{
    if (data.getDebugMode())
        Serial.println(String()+number);
}
