#include "Settings.h"
#include "Nextion/Nextion.h"
#include "DebugSerial/DebugSerial.h"
#include "FlashMemory/FlashMemory.h"
#include "Time/MyTime.h"
#include "RTOS/RTOS.h"
#include "ADC/ADS1232.h"
#include "PictureListID.h"

void Settings::mainMenu(void)
{
    bool button[7];

start:
    hmi.showPage("settings");
    hmi.waitForPageRespon();
    printDebugln("Settings page opened");

    while (true)
    {
        // hmi.serialEvent_2();
        if (hmi.getExitPageFlag())
        {
            printDebugln("Exit Settings page");
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
                    printDebugln("Timezone page opened");
                    timeZone();
                    break;
                case 1:
                    printDebugln("Brightness page opened");
                    brightness();
                    break;
                case 2:
                    printDebugln("Maximum page opened");
                    maximumWeight();
                    break;
                case 3:
                    printDebugln("Set battery capacity");
                    batteryCapacity();
                    break;
                case 4:
                    printDebugln("Debug page opened");
                    debugMenu();
                    break;
                case 5:
                    printDebugln("Calibration page opened");
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
        hmi.setIntegerToNextion("b0.picc", Timezone_Menu_Selected_Bkg);
        hmi.setIntegerToNextion("b1.picc", Timezone_Menu_Normal_Btn);
        hmi.setIntegerToNextion("b2.picc", Timezone_Menu_Normal_Btn);
        break;
    case 1:
        hmi.setIntegerToNextion("b1.picc", Timezone_Menu_Selected_Bkg);
        hmi.setIntegerToNextion("b0.picc", Timezone_Menu_Normal_Btn);
        hmi.setIntegerToNextion("b2.picc", Timezone_Menu_Normal_Btn);
        break;
    case 2:
        hmi.setIntegerToNextion("b2.picc", Timezone_Menu_Selected_Bkg);
        hmi.setIntegerToNextion("b0.picc", Timezone_Menu_Normal_Btn);
        hmi.setIntegerToNextion("b1.picc", Timezone_Menu_Normal_Btn);
        break;
    default:
        hmi.setIntegerToNextion("b0.picc", Timezone_Menu_Normal_Btn);
        hmi.setIntegerToNextion("b1.picc", Timezone_Menu_Normal_Btn);
        hmi.setIntegerToNextion("b2.picc", Timezone_Menu_Normal_Btn);
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
            printDebugln("Exit Timezone page");
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
                    printDebugln("UTC 07:00 selected");
                    break;
                case 1:
                    printDebugln("UTC 08:00 selected");
                    break;
                case 2:
                    printDebugln("UTC 09:00 selected");
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
            printDebugln("Exit Maximum page");
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
                    printDebugln(String() + "CH" + (i + 1) + " Maximum set to " + newValueStr);
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
        printDebugln(String() + "Battery capacity set: " + data.getBatteryCapacity() + " mAh");
    }
}

void Settings::showBaudrateOption(bool type, bool show)
{
    if (type == debugging)
        hmi.setVisObjectNextion("q1", show);
    for (uint i = 0; i < 10; i++)
        hmi.setVisObjectNextion(String() + "b" + i, show);
}

void Settings::updateSelectedBaudrateToNextion(bool type, uint8_t selected)
{
    data.setBaudrateSerial(type, selected);
    printDebugln(String() + "Baudrate set: " + data.getBaudrateSerial(type));

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
    showBaudrateOption(debugging, enDisDebug);
    if (enDisDebug)
        updateSelectedBaudrateToNextion(debugging, data.getBaudrateSerialIndex(debugging));

    while (true)
    {
        // hmi.serialEvent_2();
        if (hmi.getExitPageFlag())
        {
            printDebugln("Exit Debug page");
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
                    showBaudrateOption(debugging, enDisDebug);
                    if (enDisDebug)
                    {
                        updateSelectedBaudrateToNextion(debugging, data.getBaudrateSerialIndex(debugging));
                        Serial.begin(data.getBaudrateSerial(debugging));
                        while (!Serial)
                            ;
                    }
                    else
                        Serial.end();
                    printDebugln(String() + "Debug mode " + (enDisDebug ? "Enabled" : "Disabled"));
                    break;
                default:
                    Serial.end();
                    updateSelectedBaudrateToNextion(debugging, i);
                    Serial.begin(data.getBaudrateSerial(debugging));
                    while (!Serial)
                        ;
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
            printDebugln("Exit Calibration page");
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
            printDebugln("Exit Setpoint page");
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
                        printDebugln(String() + "Ch" + channel + " Point " + (i + 1) + " set to " + newValueStr + " grams");
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
                    if (progress < 10)
                    {
                        data.setAdcCalibrationPoint(0, 0, ads.adcRead[0][0]);
                        data.setAdcCalibrationPoint(1, 0, ads.adcRead[0][1]);
                        data.setAdcCalibrationPoint(2, 0, ads.adcRead[1][0]);
                        data.setAdcCalibrationPoint(3, 0, ads.adcRead[1][1]);
                        data.setAdcCalibrationPoint(4, 0, ads.adcRead[2][0]);
                        data.setAdcCalibrationPoint(5, 0, ads.adcRead[2][1]);
                    }
                    hmi.setIntegerToNextion("saving.val", progress);
                    progress += 10;
                    delay(50);
                }
                delay(500);
                hmi.setVisObjectNextion("saving", false);
            }
        }
        if (hmi.getExitPageFlag())
        {
            printDebugln("Exit Zero Calibration page");
            break;
        }
    }
}

void updatePointCalibParameter(uint8_t channel, uint8_t point)
{
    if (point > 0)
    {
        hmi.setStringToNextion("t0.txt", String() + data.getGramCalibrationPoint(channel, point - 1));
    }
    else
    {
        hmi.setStringToNextion("t0.txt", String() + static_cast<float>(0));
    }
    if (data.getPointCalibrationStatus(channel, point))
    {
        hmi.setIntegerToNextion("t2.picc", 49);
        hmi.setIntegerToNextion("t2.picc2", 51);
    }
    else
    {
        hmi.setIntegerToNextion("t2.picc", 48);
        hmi.setIntegerToNextion("t2.picc2", 50);
    }
}

void Settings::pointCalibration(void)
{
    bool button[6];
    int8_t channelState = 0;
    int8_t pointState = 0;
    uint8_t settingState = 0;
    uint8_t temp = 0;
    String prevStr = "";
    String newStr = "";
    uint32_t temp_adc = 0;

    hmi.showPage("pointcal");
    hmi.waitForPageRespon();

    updatePointCalibParameter(channelState, pointState);

    while (true)
    {
        if (settingState)
        {
            if (channelState % 2)
            {
                temp = channelState == 1 ? 0 : channelState == 3 ? 1
                                           : channelState == 5   ? 2
                                                                 : 0;
                newStr = ads.adcReadString[temp][1];
                temp_adc = ads.adcRead[temp][1];
            }
            else
            {
                temp = channelState == 0 ? 0 : channelState == 2 ? 1
                                           : channelState == 4   ? 2
                                                                 : 0;
                newStr = ads.adcReadString[temp][0];
                temp_adc = ads.adcRead[temp][0];
            }
            if (newStr != prevStr)
            {
                hmi.setStringToNextion("t5.txt", newStr);
                prevStr = newStr;
            }
        }

        for (int i = 0; i < 6; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 0:
                    if (++channelState >= (MAX_CHANNEL - 1))
                        channelState = (MAX_CHANNEL - 1);
                    hmi.setStringToNextion("t1.txt", String() + (channelState + 1));
                    printDebugln(String() + "Channel " + (channelState + 1) + " selected");
                    settingState = 0;
                    break;
                case 1:
                    if (--channelState <= 0)
                        channelState = 0;
                    hmi.setStringToNextion("t1.txt", String() + (channelState + 1));
                    printDebugln(String() + "Channel " + (channelState + 1) + " selected");
                    settingState = 0;
                    break;
                case 2:
                    if (++pointState >= (MAX_POINT_CAL - 1))
                        pointState = (MAX_POINT_CAL - 1);
                    hmi.setStringToNextion("t2.txt", String() + (pointState + 1));
                    printDebugln(String() + "Point " + (pointState + 1) + " selected");
                    settingState = 0;
                    break;
                case 3:
                    if (--pointState <= 0)
                        pointState = 0;
                    hmi.setStringToNextion("t2.txt", String() + (pointState + 1));
                    printDebugln(String() + "Point " + (pointState + 1) + " selected");
                    settingState = 0;
                    break;
                case 4:
                    if (settingState == 0)
                    {
                        settingState = 1;
                        hmi.setIntegerToNextion("b4.picc", 49);
                        hmi.setIntegerToNextion("b4.picc2", 51);
                        hmi.setIntegerToNextion("q0.picc", 49);
                    }
                    else if (settingState == 1)
                    {
                        settingState = 0;

                        data.setAdcCalibrationPoint(channelState, pointState, temp_adc);
                        if ((((data.getAdcCalibrationPoint(channelState, pointState) > data.getAdcCalibrationPoint(channelState, pointState - 1)) && pointState > 0) &&
                             ((data.getAdcCalibrationPoint(channelState, pointState) < data.getAdcCalibrationPoint(channelState, pointState + 1)) && pointState < (MAX_CHANNEL - 1))) ||
                            ((data.getAdcCalibrationPoint(channelState, pointState) > data.getAdcCalibrationPoint(channelState, pointState - 1)) && pointState == (MAX_CHANNEL - 1)) ||
                            ((pointState == 0) && (data.getAdcCalibrationPoint(channelState, pointState) > 0)))
                            data.setPointCalibrationStatus(channelState, pointState, true);
                        else
                            data.setPointCalibrationStatus(channelState, pointState, false);

                        hmi.setIntegerToNextion("b4.picc", 48);
                        hmi.setIntegerToNextion("b4.picc2", 50);
                        hmi.setIntegerToNextion("q0.picc", 48);
                    }
                    break;
                case 5:
                    data.setPointCalibrationStatus(channelState, pointState, false);
                    break;
                default:
                    break;
                }
                updatePointCalibParameter(channelState, pointState);
                if (i != 4)
                {
                    if (settingState == 0)
                    {
                        hmi.setIntegerToNextion("b4.picc", 48);
                        hmi.setIntegerToNextion("b4.picc2", 50);
                        hmi.setIntegerToNextion("q0.picc", 48);
                    }
                }
            }
        }
        if (hmi.getExitPageFlag())
        {
            printDebugln("Exit Point Calibration page");
            break;
        }
    }
}

Settings setting;