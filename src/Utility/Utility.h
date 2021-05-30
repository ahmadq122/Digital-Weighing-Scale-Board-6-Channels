#pragma once

#include "Arduino.h"

#define color_white 65535
#define color_cyan 2047
#define color_grey 46518
#define color_black 0
#define color_red 63488

class Utility
{
public:
    uint8_t charArraySize(const char *charArray);
    void integerToString(uint32_t number, char *buffer, uint8_t len);
    String integerToString(uint32_t number, uint8_t len);
    void floatToString(double Number, char *Str, unsigned char NoOfDigitsBeforePoint, unsigned char NoOfDigitsAfterPoint);
    String floatToString(double Number, unsigned char NoOfDigitsBeforePoint, unsigned char NoOfDigitsAfterPoint);

private:
    double generateMultiplier(unsigned char parameter);
};

extern Utility utils;