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

void Utility::floatToString(double Number, char *Str, unsigned char NoOfDigitsBeforePoint, unsigned char NoOfDigitsAfterPoint)
{
    NoOfDigitsAfterPoint = constrain(NoOfDigitsAfterPoint, 0, 5);
    unsigned long temp1 = static_cast<unsigned long>(Number);
    double temp2 = (Number - static_cast<double>(temp1)) * generateMultiplier(NoOfDigitsAfterPoint);
    unsigned long temp3 = static_cast<unsigned long>(temp2);

    integerToString(temp1, Str, NoOfDigitsBeforePoint);

    if (NoOfDigitsAfterPoint != 0)
    {
        Str[NoOfDigitsBeforePoint] = '.';
        integerToString(temp3, &Str[NoOfDigitsBeforePoint + 1], NoOfDigitsAfterPoint);
        Str[NoOfDigitsBeforePoint + 1 + NoOfDigitsAfterPoint] = '\0';
    }
    else
        Str[NoOfDigitsBeforePoint] = '\0';
}

String Utility::floatToString(double Number, unsigned char NoOfDigitsBeforePoint, unsigned char NoOfDigitsAfterPoint)
{
    char Str[NoOfDigitsBeforePoint + NoOfDigitsAfterPoint + 2];
    unsigned long temp1 = static_cast<unsigned long>(Number);
    double temp2 = (Number - static_cast<double>(temp1)) * generateMultiplier(NoOfDigitsAfterPoint);
    unsigned long temp3 = static_cast<unsigned long>(temp2);

    NoOfDigitsAfterPoint = constrain(NoOfDigitsAfterPoint, 0, 5);

    integerToString(temp1, Str, NoOfDigitsBeforePoint);

    if (NoOfDigitsAfterPoint != 0)
    {
        Str[NoOfDigitsBeforePoint] = '.';
        integerToString(temp3, &Str[NoOfDigitsBeforePoint + 1], NoOfDigitsAfterPoint);
        Str[NoOfDigitsBeforePoint + 1 + NoOfDigitsAfterPoint] = '\0';
    }
    else
        Str[NoOfDigitsBeforePoint] = '\0';
    return Str;
}

double Utility::generateMultiplier(unsigned char parameter)
{
    parameter = constrain(parameter, 1, 5);
    if (parameter == 1)
        return 10;
    else if (parameter == 2)
        return 100;
    else if (parameter == 3)
        return 1000;
    else if (parameter == 4)
        return 10000;
    else if (parameter == 5)
        return 100000;
    else
        return 0;
}

Utility utils;