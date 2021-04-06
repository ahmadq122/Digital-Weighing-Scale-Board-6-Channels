#include "Utility.h"

uint8_t Utility::charArraySize(const char *charArray)
{
    unsigned int index = 0;

    for (index = 0; index < 0xFF; index++)
    {
        if (charArray[index] == '\0')
            return index;
    }
    return 0;
}

Utility utils;