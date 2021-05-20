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

void Utility::integerToString(uint32_t number, char *buffer, uint8_t len)
{
    String tempStr = String() + number;
    String numbStr;
    char charFirstZero[10];
    uint8_t a = 0, b = 0;

    if (tempStr.length() < len)
    {
        a = len - tempStr.length();
        while (a)
        {
            charFirstZero[b] = '0';
            a--;
            b++;
        }
        charFirstZero[b] = '\0';
        numbStr = String() + charFirstZero;
        numbStr += tempStr;
    }
    else
    {
        numbStr = tempStr;
    }
    strcpy(buffer, numbStr.c_str());
}

String Utility::integerToString(uint32_t number, uint8_t len)
{
    String tempStr = String() + number;
    String numbStr;
    char charFirstZero[10];
    uint8_t a = 0, b = 0;

    if (tempStr.length() < len)
    {
        a = len - tempStr.length();
        while (a)
        {
            charFirstZero[b] = '0';
            a--;
            b++;
        }
        charFirstZero[b] = '\0';
        numbStr = String() + charFirstZero;
        numbStr += tempStr;
    }
    else
    {
        numbStr = tempStr;
    }
    return numbStr;
}

Utility utils;