#include "Settings.h"
#include "Nextion/Nextion.h"
#include "DebugSerial/DebugSerial.h"
#include "FlashMemory/FlashMemory.h"
#include "Time/MyTime.h"
#include "RTOS/RTOS.h"
#include "ADC/ADS1232.h"

void Settings::settingsMenu(void)
{
    bool button[7];

start:
    hmi.showPage("settings");
    hmi.waitForPageRespon();
    printDebug("Settings page opened");

    while (true)
    {
        // hmi.serialEvent_2();
        if (hmi.getExitPageFlag())
        {
            printDebug("Exit Settings page");
            return;
        }
        for (int i = 0; i < 6; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 0:
                    printDebug("Timezone page opened");
                    timeZone();
                    break;
                case 1:
                    printDebug("Brightness page opened");
                    brightness();
                    break;
                case 2:
                    printDebug("Maximum page opened");
                    maximumWeight();
                    break;
                case 3:
                    printDebug("Set battery capacity");
                    batteryCapacity();
                    break;
                case 4:
                    printDebug("Debug page opened");
                    debugMenu();
                    break;
                case 5:
                    printDebug("Calibration page opened");
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
        hmi.setIntegerToNextion("b0.picc", 29);
        hmi.setIntegerToNextion("b1.picc", 27);
        hmi.setIntegerToNextion("b2.picc", 27);
        break;
    case 1:
        hmi.setIntegerToNextion("b1.picc", 29);
        hmi.setIntegerToNextion("b0.picc", 27);
        hmi.setIntegerToNextion("b2.picc", 27);
        break;
    case 2:
        hmi.setIntegerToNextion("b2.picc", 29);
        hmi.setIntegerToNextion("b0.picc", 27);
        hmi.setIntegerToNextion("b1.picc", 27);
        break;
    default:
        hmi.setIntegerToNextion("b0.picc", 27);
        hmi.setIntegerToNextion("b1.picc", 27);
        hmi.setIntegerToNextion("b2.picc", 27);
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
        if (hmi.getExitPageFlag())
        {
            printDebug("Exit Timezone page");
            return;
        }
        for (int i = 0; i < 4; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 0:
                    printDebug("UTC 07:00 selected");
                    break;
                case 1:
                    printDebug("UTC 08:00 selected");
                    break;
                case 2:
                    printDebug("UTC 09:00 selected");
                    break;
                default:
                    break;
                }
                data.setTimezone(i);
                updateSelectedTimezoneToNextion();
            }
        }
    }
}

void Settings::brightness(void)
{
    bool button[7];
    bool popup = false;

    hmi.showPage("bright");
    hmi.waitForPageRespon();
    if (data.getDimScreenTimer() == 0)
    {
        hmi.setVisObjectNextion("q1", false);
        hmi.setStringToNextion("b0.txt", "Never");
    }
    else
    {
        hmi.setStringToNextion("b0.txt", String() + data.getDimScreenTimer());
        hmi.setVisObjectNextion("q1", true);
    }

    while (!hmi.getExitPageFlag())
    {
        if (rtos.dimmCounterDownSecond == 0 && data.getDimScreenTimer() > 0)
        {
            hmi.setIntegerToNextion("h0.val", rtos.currentBrightness);
        }
        for (int i = 0; i < 7; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 0:
                    popup = !popup;
                    hmi.setVisObjectNextion("q2", popup);
                    for (uint8_t i = 1; i <= 6; i++)
                    {
                        hmi.setVisObjectNextion(String() + "b" + i, popup);
                    }
                    break;
                default:
                    data.setDimScreenTimer(i - 1);
                    hmi.setVisObjectNextion("q2", false);
                    for (uint8_t i = 1; i <= 6; i++)
                    {
                        hmi.setVisObjectNextion(String() + "b" + i, false);
                    }
                    if (data.getDimScreenTimer() == 0)
                    {
                        hmi.setVisObjectNextion("q1", false);
                        hmi.setStringToNextion("b0.txt", "Never");
                    }
                    else
                    {
                        hmi.setStringToNextion("b0.txt", String() + data.getDimScreenTimer());
                        hmi.setVisObjectNextion("q1", true);
                    }
                    break;
                }
            }
        }
    }
    data.setScreenBrightness(hmi.getDataInteger(0));
}

void Settings::updateMaximumValueToNextion(void)
{
    for (uint8_t i = 0; i < 6; i++)
    {
        hmi.setStringToNextion(String() + "b" + i + ".txt", String() + data.getGramMaximum(i));
    }
}
void Settings::maximumWeight(void)
{
    bool button[6];
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
        if (hmi.getExitPageFlag())
        {
            printDebug("Exit Maximum page");
            return;
        }
        for (int i = 0; i < 6; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                hmi.showPage("numpad");
                hmi.waitForPageRespon();
                hmi.setStringToNextion("num_string.txt", String() + data.getGramMaximum(i));
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
                    printDebug(String() + "CH" + (i + 1) + " Maximum set to " + newValueStr);
                    data.setGramMaximum(i, newValue);
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
        printDebug(String() + "Battery capacity set: " + data.getBatteryCapacity() + " mAh");
    }
}

void Settings::showBaudrateOption(bool type, bool show)
{
    if (type == DEBUG)
        hmi.setVisObjectNextion("q1", show);
    for (uint i = 0; i < 10; i++)
        hmi.setVisObjectNextion(String() + "b" + i, show);
}

void Settings::updateSelectedBaudrateToNextion(bool type, uint8_t selected)
{
    data.setBaudrateSerial(type, selected);
    printDebug(String() + "Baudrate set: " + data.getBaudrateSerial(type));

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
        updateSelectedBaudrateToNextion(DEBUG, data.getBaudrateSerialIndex(DEBUG));

    while (true)
    {
        // hmi.serialEvent_2();
        if (hmi.getExitPageFlag())
        {
            printDebug("Exit Debug page");
            return;
        }
        for (int i = 0; i < 12; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 10:
                    enDisDebug = !enDisDebug;
                    data.setDebugMode(enDisDebug);
                    hmi.setIntegerToNextion("bt0.val", enDisDebug);
                    showBaudrateOption(DEBUG, enDisDebug);
                    if (enDisDebug)
                    {
                        updateSelectedBaudrateToNextion(DEBUG, data.getBaudrateSerialIndex(DEBUG));
                        Serial.begin(data.getBaudrateSerial(DEBUG));
                        while (!Serial)
                            ;
                    }
                    else
                        Serial.end();
                    printDebug(String() + "Debug mode " + (enDisDebug ? "Enabled" : "Disabled"));
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
    bool button[3];

start:
    hmi.showPage("calib");
    hmi.waitForPageRespon();
    while (true)
    {
        // hmi.serialEvent_2();
        if (hmi.getExitPageFlag())
        {
            printDebug("Exit Calibration page");
            return;
        }
        for (int i = 0; i < 3; i++)
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
            valid[i] &= ((data.getGramCalibrationPoint(channel, i) > 0) && (data.getGramCalibrationPoint(channel, i) < data.getGramMaximum(channel)));
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
    bool button[9];
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
        if (hmi.getExitPageFlag())
        {
            printDebug("Exit Setpoint page");
            return;
        }
        for (int i = 0; i < 9; i++)
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
                        printDebug(String() + "Ch" + channel + " Point " + (i + 1) + " set to " + newValueStr + " grams");
                        data.setGramCalibrationPoint(channel - 1, i, newValue);
                    }
                    break;
                }
                goto start;
            }
        }
    }
}

void updateAllAdcValue(void)
{
    hmi.setStringToNextion("t0.txt", ads.adcReadString[ads1][0]);
    hmi.setStringToNextion("t1.txt", ads.adcReadString[ads1][1]);
    hmi.setStringToNextion("t2.txt", ads.adcReadString[ads2][0]);
    hmi.setStringToNextion("t3.txt", ads.adcReadString[ads2][1]);
    hmi.setStringToNextion("t4.txt", ads.adcReadString[ads3][0]);
    hmi.setStringToNextion("t5.txt", ads.adcReadString[ads3][1]);
}

void Settings::zeroCalibration(void)
{
    uint8_t setState = 0;
    uint8_t progress = 0;
    hmi.showPage("zero");
    hmi.waitForPageRespon();

    while (true)
    {
        // hmi.serialEvent_2();
        if (hmi.getDataButton(0))
        {
            if (++setState >= 3)
                setState = 3;
            if (setState == 1)
            {
                hmi.setIntegerToNextion("b1.picc", 44);
                hmi.setIntegerToNextion("b1.picc2", 45);
                hmi.setIntegerToNextion("q1.picc", 44);
                updateAllAdcValue();
            }
            else if (setState == 2)
            {
                hmi.setVisObjectNextion("saving", true);
                while (progress <= 100)
                {
                    hmi.setIntegerToNextion("saving.val", progress);
                    progress += 10;
                    delay(100);
                }
                delay(500);
                hmi.setVisObjectNextion("saving", false);
            }
        }
        if (hmi.getExitPageFlag())
        {
            printDebug("Exit Zero Calibration page");
        break;
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
                    printDebug("Exit Point Calibration page");
                    return;
                default:
                    break;
                }
            }
        }
    }
}

Settings setting;