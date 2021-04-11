#pragma once

#include "Arduino.h"

class Utility
{
public:
    uint8_t charArraySize(const char *charArray);
    void integerToString(uint32_t number, char *buffer, uint8_t len);
};

extern Utility utils;