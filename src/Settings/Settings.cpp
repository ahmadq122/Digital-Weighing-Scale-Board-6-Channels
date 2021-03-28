#include "Settings.h"
#include "Nextion/Nextion.h"
#include "DebugSerial/DebugSerial.h"
#include "FlashMemory/FlashMemory.h"
#include "Time/MyTime.h"

void Settings::settingsMenu(void)
{
    bool button[7];

start:
    hmi.showPage("settings");
    hmi.waitForPageRespon();

    while (true)
    {
        // hmi.serialEvent_2();
        for (int i = 0; i < 7; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 0:
                    printDebug(data.getDebugMode(), "Exit Settings page");
                    return;
                case 1:
                    printDebug(data.getDebugMode(), "Timezone page opened");
                    timeZone();
                    break;
                case 2:
                    printDebug(data.getDebugMode(), "Brightness page opened");
                    brightness();
                    break;
                case 3:
                    printDebug(data.getDebugMode(), "Maximum page opened");
                    maximumWeight();
                    break;
                case 4:
                    printDebug(data.getDebugMode(), "Set battery capacity");
                    batteryCapacity();
                    break;
                case 5:
                    printDebug(data.getDebugMode(), "Debug page opened");
                    debugMenu();
                    break;
                case 6:
                    printDebug(data.getDebugMode(), "Calibration page opened");
                    calibrationSensor();
                    break;
                default:
                    break;
                }
                goto start;
            }
        }
    }
}

void Settings::updateSelectedTimezoneToNextion(void)
{
    switch (data.getTimezone())
    {
    case 0:
        hmi.setIntegerToNextion("b1.picc", 29);
        hmi.setIntegerToNextion("b2.picc", 27);
        hmi.setIntegerToNextion("b3.picc", 27);
        break;
    case 1:
        hmi.setIntegerToNextion("b2.picc", 29);
        hmi.setIntegerToNextion("b1.picc", 27);
        hmi.setIntegerToNextion("b3.picc", 27);
        break;
    case 2:
        hmi.setIntegerToNextion("b3.picc", 29);
        hmi.setIntegerToNextion("b1.picc", 27);
        hmi.setIntegerToNextion("b2.picc", 27);
        break;
    default:
        hmi.setIntegerToNextion("b1.picc", 27);
        hmi.setIntegerToNextion("b2.picc", 27);
        hmi.setIntegerToNextion("b3.picc", 27);
        break;
    }
    mtime.initOffset(data.getTimezone());
    configTime(mtime.gmtOffset_sec, mtime.daylightOffset_sec, mtime.ntpServer);
}

void Settings::timeZone(void)
{
    bool button[4];
    hmi.showPage("tzone");
    hmi.waitForPageRespon();

    updateSelectedTimezoneToNextion();

    while (true)
    {
        // hmi.serialEvent_2();
        for (int i = 0; i < 4; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 0:
                    printDebug(data.getDebugMode(), "Exit Timezone page");
                    return;
                case 1:
                    printDebug(data.getDebugMode(), "UTC 07:00 selected");
                    break;
                case 2:
                    printDebug(data.getDebugMode(), "UTC 08:00 selected");
                    break;
                case 3:
                    printDebug(data.getDebugMode(), "UTC 09:00 selected");
                    break;
                default:
                    break;
                }
                data.setTimezone(i - 1);
                updateSelectedTimezoneToNextion();
            }
        }
    }
}

void Settings::updateMaximumValueToNextion(void)
{
    for (uint8_t i = 0; i < 6; i++)
    {
        hmi.setStringToNextion(String() + "b" + i + ".txt", String() + data.getGramMaximum(i));
    }
}

void Settings::brightness(void)
{
    hmi.showPage("bright");
    hmi.waitForPageRespon();
    while (!hmi.getDataButton(0))
    {
        // hmi.serialEvent_2();
    }
}
void Settings::maximumWeight(void)
{
    bool button[7];
    bool button1[2];
    String newValueStr;
    float newValue = 0;

start:
    button1[0] = false;
    button1[1] = false;

    hmi.showPage("maximum");
    hmi.waitForPageRespon();

    updateMaximumValueToNextion();

    while (true)
    {
        // hmi.serialEvent_2();
        for (int i = 0; i < 7; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 0:
                    printDebug(data.getDebugMode(), "Exit Maximum page");
                    return;
                default:
                    hmi.showPage("numpad");
                    hmi.waitForPageRespon();
                    hmi.setStringToNextion("num_string.txt", String() + data.getGramMaximum(i - 1));
                    hmi.flushAvailableButton();
                    while (!button1[0] && !button1[1])
                    {
                        // hmi.serialEvent_2();
                        button1[0] = hmi.getDataButton(0);
                        button1[1] = hmi.getDataButton(1);
                    }
                    if (button1[1])
                    {
                        newValueStr = hmi.getDataString(0);
                        newValue = atof(newValueStr.c_str());
                        printDebug(data.getDebugMode(), String() + "CH" + i + " Maximum set to " + newValueStr);
                        data.setGramMaximum(i - 1, newValue);
                    }
                    break;
                }
                goto start;
            }
        }
    }
}
void Settings::batteryCapacity(void)
{
    bool button[2] = {false,
                      false};
    hmi.showPage("numpad");
    hmi.waitForPageRespon();
    hmi.setStringToNextion("num_string.txt", String() + data.getBatteryCapacity());
    while (!button[0] && !button[1])
    {
        // hmi.serialEvent_2();
        for (uint8_t i = 0; i < 2; i++)
            button[i] = hmi.getDataButton(i);
    }
    if (button[1])
    {
        data.setBatteryCapacity(static_cast<uint16_t>(atoi(hmi.getDataString(0))));
        printDebug(data.getDebugMode(), String() + "Battery capacity set: " + data.getBatteryCapacity() + " mAh");
    }
}

void Settings::showBaudrateOption(bool type, bool show)
{
    if (type == DEBUG)
        hmi.setVisObjectNextion("q1", show);
    for (uint i = 0; i < 10; i++)
        hmi.setVisObjectNextion(String() + "b" + i, show);
}
uint8_t Settings::getIndexSelectedBaudrate(bool type)
{
    for (uint8_t i = 0; i < 10; i++)
    {
        if (data.getBaudrateSerial(type) == baudrate[i])
            return i;
    }
    return UNKNOWN;
}
void Settings::updateSelectedBaudrateToNextion(bool type, uint8_t selected)
{
    data.setBaudrateSerial(type, baudrate[selected]);
    printDebug(data.getDebugMode(), String() + "Baudrate set: " + data.getBaudrateSerial(type));

    for (uint8_t i = 0; i < 10; i++)
    {
        if (i == selected)
            hmi.setIntegerToNextion(String() + "b" + i + ".picc", 59);
        else
            hmi.setIntegerToNextion(String() + "b" + i + ".picc", 57);
    }
}
void Settings::debugMenu(void)
{
    bool button[12];
    bool enDisDebug = data.getDebugMode();

    hmi.showPage("debug");
    hmi.waitForPageRespon();

    hmi.setIntegerToNextion("bt0.val", enDisDebug);
    showBaudrateOption(DEBUG, enDisDebug);
    if (enDisDebug)
        updateSelectedBaudrateToNextion(DEBUG, getIndexSelectedBaudrate(DEBUG));

    while (true)
    {
        // hmi.serialEvent_2();
        for (int i = 0; i < 12; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 11:
                    printDebug(data.getDebugMode(), "Exit Debug page");
                    return;
                case 10:
                    enDisDebug = !enDisDebug;
                    data.setDebugMode(enDisDebug);
                    hmi.setIntegerToNextion("bt0.val", enDisDebug);
                    showBaudrateOption(DEBUG, enDisDebug);
                    if (enDisDebug)
                    {
                        updateSelectedBaudrateToNextion(DEBUG, getIndexSelectedBaudrate(DEBUG));
                        Serial.begin(data.getBaudrateSerial(DEBUG));
                        while (!Serial)
                            ;
                    }
                    else
                        Serial.end();
                    printDebug(data.getDebugMode(), String() + "Debug mode " + (enDisDebug ? "Enabled" : "Disabled"));
                    break;
                default:
                    updateSelectedBaudrateToNextion(DEBUG, i);
                    break;
                }
            }
        }
    }
}

void Settings::calibrationSensor(void)
{
    bool button[4];

start:
    hmi.showPage("calib");
    hmi.waitForPageRespon();
    while (true)
    {
        // hmi.serialEvent_2();
        for (int i = 0; i < 4; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 0:
                    setPoint();
                    break;
                case 1:
                    zeroCalibration();
                    break;
                case 2:
                    pointCalibration();
                    break;
                case 3:
                    printDebug(data.getDebugMode(), "Exit Calibration page");
                    return;
                default:
                    break;
                }
                goto start;
            }
        }
    }
}

void updateSetpointToNextion(uint8_t channel)
{
    bool valid[7];
    uint8_t i = 0;

    for (i = 0; i < 7; i++)
        valid[i] = true;
    for (i = 0; i < 7; i++)
    {
        if (i == 0)
            valid[i] &= (data.getGramCalibrationPoint(channel, i) < data.getGramMaximum(channel));
        else
        {
            if (data.getGramCalibrationPoint(channel, i) > 0 || ((data.getGramCalibrationPoint(channel, i + 1) > 0) && i < 6))
                valid[i] &= ((data.getGramCalibrationPoint(channel, i) > data.getGramCalibrationPoint(channel, i - 1)) && (data.getGramCalibrationPoint(channel, i) <= data.getGramMaximum(channel)));
        }
    }

    for (i = 0; i < 7; i++)
    {
        hmi.setStringToNextion(String() + "b" + i + ".txt", String() + data.getGramCalibrationPoint(channel, i));
        if (valid[i])
            hmi.setIntegerToNextion(String() + "b" + i + ".pco", 0); //set font color to red
        else
            hmi.setIntegerToNextion(String() + "b" + i + ".pco", 63488); //set font color to black
    }
}

void Settings::setPoint(void)
{
    bool button[10];
    bool button1[2];
    String newValueStr;
    float newValue = 0;
    uint8_t channel = 1;

start:
    button1[0] = false;
    button1[1] = false;
    hmi.showPage("setpoint");
    hmi.waitForPageRespon();
    hmi.setStringToNextion("t0.txt", String() + channel);
    updateSetpointToNextion(channel - 1);

    while (true)
    {
        // hmi.serialEvent_2();
        for (int i = 0; i < 10; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 7:
                    if (++channel > 6)
                        channel = 6;
                    break;
                case 8:
                    if (--channel < 1)
                        channel = 1;
                    break;
                case 9:
                    printDebug(data.getDebugMode(), "Exit Setpoint page");
                    return;
                default:
                    hmi.showPage("numpad");
                    hmi.waitForPageRespon();
                    hmi.setStringToNextion("num_string.txt", String() + data.getGramCalibrationPoint(channel - 1, i));
                    hmi.flushAvailableButton();
                    while (!button1[0] && !button1[1])
                    {
                        // hmi.serialEvent_2();
                        button1[0] = hmi.getDataButton(0);
                        button1[1] = hmi.getDataButton(1);
                    }
                    if (button1[1])
                    {
                        newValueStr = hmi.getDataString(0);
                        newValue = atof(newValueStr.c_str());
                        printDebug(data.getDebugMode(), String() + "Ch" + channel + " Point " + (i + 1) + " set to " + newValueStr + " grams");
                        data.setGramCalibrationPoint(channel - 1, i, newValue);
                    }
                    break;
                }
                goto start;
            }
        }
    }
}
void Settings::zeroCalibration(void)
{
    bool button[2];
    hmi.showPage("zero");
    hmi.waitForPageRespon();
    while (true)
    {
        // hmi.serialEvent_2();
        for (int i = 0; i < 2; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 1:
                    printDebug(data.getDebugMode(), "Exit Zero Calibration page");
                    return;
                default:
                    break;
                }
            }
        }
    }
}
void Settings::pointCalibration(void)
{
    bool button[6];

    hmi.showPage("pointcal");
    hmi.waitForPageRespon();
    while (true)
    {
        // hmi.serialEvent_2();
        for (int i = 0; i < 6; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 5:
                    printDebug(data.getDebugMode(), "Exit Point Calibration page");
                    return;
                default:
                    break;
                }
            }
        }
    }
}

Settings setting;