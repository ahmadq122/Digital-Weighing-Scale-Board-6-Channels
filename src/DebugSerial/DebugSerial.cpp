#include "DebugSerial.h"
#include "FlashMemory/FlashMemory.h"

void printDebugln(const char *string)
{
    if (data.getDebugMode())
        Serial.println(F(string));
}

void printDebugln(String string)
{
    if (data.getDebugMode())
        Serial.println(string);
}

void printDebugln(long number)
{
    if (data.getDebugMode())
        Serial.println(String()+number);
}

void printDebug(const char *string)
{
    if (data.getDebugMode())
        Serial.print(F(string));
}

void printDebug(String string)
{
    if (data.getDebugMode())
        Serial.print(string);
}

void printDebug(long number)
{
    if (data.getDebugMode())
        Serial.print(String()+number);
}