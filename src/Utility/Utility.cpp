#include "Utility.h"
#include "PinMap.h"
#include "DebugSerial/DebugSerial.h"
#include "RTOS/RTOS.h"
#include "FlashMemory/FlashMemory.h"

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

uint8_t Utility::getBatteryPercent(void)
{
    float adcBattery = 0;
    float percent = 0.0;
    float voltageDivider = 0.0;
    float voltageActual = 0.0;
    float attenuation = 0.0;
    static uint8_t index = 0;
    float min = data.getMinimumBattery();
    float max = data.getMaximumBattery();

    attenuation = 0.3188; //(Resistor_Two_VoltDiv / (Resistor_One_VoltDiv + Resistor_Two_VoltDiv));

    //USE MOVING AVERAGE TERM

    if (index < ADCsample)
    {
        adcBatteryContainer[index] = analogRead(Pin_VBat_Sense);
        delay(10);
        index++;
        return 0;
    }
    else
    {
        if (rtos.secondTriggered[6])
        {
            for (uint8_t i = 0; i < (ADCsample - 1); i++)
            {
                adcBatteryContainer[i] = adcBatteryContainer[i + 1];
            }
            adcBatteryContainer[ADCsample - 1] = analogRead(Pin_VBat_Sense);
            delay(10);
            for (uint8_t i = 0; i < ADCsample; i++)
            {
                adcBattery += adcBatteryContainer[i];
            }

            adcBattery /= ADCsample;
            voltageDivider = (adcBattery / 4095.0) * ESP_Analog_Voltage_Refrence;

            if (adcBattery > 10)
            {
                if (data.getDebugMode())
                    voltageDivider += 0.115; //correction
                else
                    voltageDivider += 0.175; //correction
            }

            voltageActual = voltageDivider / attenuation;
            vBatActual = voltageActual;

            percent = ((constrain((voltageActual - min), 0.0, (max - min))) / (max - min)) * 100.0;

            // printDebugln(String() + "---------------------------");
            // // printDebugln(String() + "Attenuation : " + attenuation);
            // printDebugln(String() + "Voltage ADC : " + adcBattery);
            // printDebugln(String() + "Voltage Actual : " + voltageActual + "V");
            // printDebugln(String() + "Voltage Divider : " + voltageDivider + "V");
            // printDebugln(String() + "Battey Percent : " + percent + "%");
            // printDebugln(String() + "---------------------------");
            rtos.secondTriggered[6] = false;

            return (uint8_t)percent;
        }
    }
    return 0xFF;
}

Utility utils;