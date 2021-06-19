#pragma once

#include "Arduino.h"

#define color_white 65535
#define color_cyan 2047
#define color_grey 46518
#define color_black 0
#define color_red 63488

#define ADCsample 10
#define ESP_Analog_Voltage_Refrence 3.3
// #define Minimum_Battery_Voltage 7.00
// #define Maximum_Battery_Voltage 8.40 //2 cells 3.7V Serial
#define Resistor_One_VoltDiv 47000   //Connected to Battery(+)
#define Resistor_Two_VoltDiv 22000   //Connected to GND
#define Timer_Second_Battery_Low 60 

class Utility
{
public:
    uint8_t charArraySize(const char *charArray);
    void integerToString(uint32_t number, char *buffer, uint8_t len);
    String integerToString(uint32_t number, uint8_t len);
    void floatToString(double Number, char *Str, unsigned char NoOfDigitsBeforePoint, unsigned char NoOfDigitsAfterPoint);
    String floatToString(double Number, unsigned char NoOfDigitsBeforePoint, unsigned char NoOfDigitsAfterPoint);
    uint8_t getBatteryPercent(void);
    float vBatActual = 0.0;

private:
    double generateMultiplier(unsigned char parameter);
    uint16_t adcBatteryContainer[ADCsample];
};

extern Utility utils;