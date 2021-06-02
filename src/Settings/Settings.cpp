#include "Settings.h"
#include "Nextion/Nextion.h"
#include "DebugSerial/DebugSerial.h"
#include "FlashMemory/FlashMemory.h"
#include "Time/MyTime.h"
#include "RTOS/RTOS.h"
#include "ADC/ADS1232.h"
#include "PictureListID.h"
#include "Utility/Utility.h"

void Settings::mainMenu(void)
{
    bool button[8];
    bool adcRateValue = data.getSpeedRate();
    bool mute = data.getBuzzerMute();
    uint8_t menuPage = 0;

start:

    if (menuPage == 0)
    {
        hmi.showPage("settings");
        hmi.waitForPageRespon();
    }
    else
    {
        hmi.showPage("settings1");
        hmi.waitForPageRespon();
        if (adcRateValue && data.getSpeedRate())
        {
            hmi.setIntegerToNextion("b0.picc", Settings1_Menu_Normal_Btn);
            hmi.setIntegerToNextion("b0.picc2", Settings1_Menu_Prs_Btn);
        }
        else
        {
            if (adcRateValue != data.getSpeedRate())
                adcRateValue = data.getSpeedRate();
            hmi.setIntegerToNextion("b0.picc", Settings11_Menu_Normal_Btn);
            hmi.setIntegerToNextion("b0.picc2", Settings11_Menu_Prs_Btn);
        }
        if (mute && data.getBuzzerMute())
        {
            hmi.setIntegerToNextion("b2.picc", Settings11_Menu_Normal_Btn);
            hmi.setIntegerToNextion("b2.picc2", Settings11_Menu_Prs_Btn);
        }
        else
        {
            if (mute != data.getBuzzerMute())
                mute = data.getBuzzerMute();
            hmi.setIntegerToNextion("b2.picc", Settings1_Menu_Normal_Btn);
            hmi.setIntegerToNextion("b2.picc2", Settings1_Menu_Prs_Btn);
        }
    }
    printDebugln("Settings page opened");

    while (true)
    {
        // hmi.serialEvent_2();
        if (hmi.getExitPageFlag())
        {
            printDebugln("Exit Settings page");
            return;
        }
        for (int i = 0; i < 8; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 0:
                    if (menuPage == 0)
                    {
                        printDebugln("Timezone page opened");
                        timeZone();
                    }
                    else if (menuPage == 1)
                    {
                        printDebugln("ADC Rate value changed");
                        adcRateValue = !adcRateValue;
                        data.setSpeedRate(adcRateValue);
                        hmi.showSavingBarAnimation(1000);
                    }
                    break;
                case 1:
                    printDebugln("Brightness page opened");
                    if (menuPage == 0)
                        brightness();
                    else if (menuPage == 1)
                    {
                        hmi.setVisObjectNextion("q0", true);
                        hmi.setVisObjectNextion("b8", true);
                        hmi.setVisObjectNextion("b9", true);

                        while (true)
                        {
                            if (hmi.getExitPageFlag())
                            {
                                printDebugln("Exit Settings page");
                                return;
                            }
                            if (hmi.getDataButton(8))
                            {
                                break;
                            }
                            if (hmi.getDataButton(9))
                            {
                                if (data.resetDefault())
                                    hmi.showSavingBarAnimation(250);
                                break;
                            }
                        }

                        hmi.setVisObjectNextion("b8", false);
                        hmi.setVisObjectNextion("b9", false);
                        hmi.setVisObjectNextion("q0", false);
                        hmi.flushAvailableButton();
                    }
                    break;
                case 2:
                    printDebugln("Maximum page opened");
                    if (menuPage == 0)
                        maximumWeight();
                    else
                    {
                        mute = !mute;
                        data.setBuzzerMute(mute);
                        hmi.showSavingBarAnimation(500);
                    }
                    break;
                case 3:
                    printDebugln("Set time and date");
                    if (menuPage == 0)
                        timeAndDate();
                    break;
                case 4:
                    printDebugln("Debug page opened");
                    if (menuPage == 0)
                        debugMenu();
                    break;
                case 5:
                    printDebugln("Calibration page opened");
                    if (menuPage == 0)
                        calibrationSensor();
                    break;
                case 6:
                    printDebugln("Left menu page");
                    if (menuPage == 1)
                    {
                        // hmi.showPage("settings");
                        // hmi.waitForPageRespon();
                        menuPage = 0;
                    }
                    break;
                case 7:
                    printDebugln("Right menu page");
                    if (menuPage == 0)
                    {
                        // hmi.showPage("settings1");
                        // hmi.waitForPageRespon();
                        menuPage = 1;
                    }
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
                    for (uint8_t a = 1; a <= 6; a++)
                    {
                        hmi.setVisObjectNextion(String() + "b" + a, popup);
                    }
                    break;
                default:
                    data.setDimScreenTimer(i - 1);
                    rtos.dimmCounterDownSecond = data.getDimScreenTimer();
                    hmi.setVisObjectNextion("q2", false);
                    for (uint8_t a = 1; a <= 6; a++)
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
    bool button1[2] = {false};
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
void Settings::timeAndDate(void)
{
    bool button[12];
    int8_t data[6]; //hour,minute,second,date,month,year
    mtime.getRtcTime(&data[0], &data[1], &data[2]);
    mtime.getRtcDate(&data[3], &data[4], &data[5]);

    hmi.showPage("timedate");
    hmi.waitForPageRespon();

    for (uint8_t i = 0; i < 6; i++)
    {
        if (i == 2)
            continue;
        hmi.setStringToNextion(String() + "t" + i + ".txt", utils.integerToString(data[i], 2));
    }

    while (true)
    {
        if (hmi.getExitPageFlag())
        {
            printDebugln("Exit Time and Date page");
            return;
        }
        for (byte i = 0; i < 12; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 0:
                    if (++data[0] > 23)
                        data[0] = 0;
                    hmi.setStringToNextion("t0.txt", utils.integerToString(data[0], 2));
                    break;
                case 1:
                    if (--data[0] < 0)
                        data[0] = 23;
                    hmi.setStringToNextion("t0.txt", utils.integerToString(data[0], 2));
                    break;
                case 2:
                    if (++data[1] > 59)
                        data[1] = 0;
                    hmi.setStringToNextion("t1.txt", utils.integerToString(data[1], 2));
                    break;
                case 3:
                    if (--data[1] < 0)
                        data[1] = 59;
                    hmi.setStringToNextion("t1.txt", utils.integerToString(data[1], 2));
                    break;
                case 4:
                    if (++data[3] > 31)
                        data[3] = 1;
                    hmi.setStringToNextion("t3.txt", utils.integerToString(data[3], 2));
                    break;
                case 5:
                    if (--data[3] < 1)
                        data[3] = 31;
                    hmi.setStringToNextion("t3.txt", utils.integerToString(data[3], 2));
                    break;
                case 6:
                    if (++data[4] > 12)
                        data[4] = 1;
                    hmi.setStringToNextion("t4.txt", utils.integerToString(data[4], 2));
                    break;
                case 7:
                    if (--data[4] < 1)
                        data[4] = 12;
                    hmi.setStringToNextion("t4.txt", utils.integerToString(data[4], 2));
                    break;
                case 8:
                    if (++data[5] > 99)
                        data[5] = 0;
                    hmi.setStringToNextion("t5.txt", utils.integerToString(data[5], 2));
                    break;
                case 9:
                    if (--data[5] < 0)
                        data[5] = 99;
                    hmi.setStringToNextion("t5.txt", utils.integerToString(data[5], 2));
                    break;
                case 10:
                    mtime.setRtcTime(data[0], data[1], 0);
                    hmi.showSavingBarAnimation(500);
                    break;
                case 11:
                    mtime.setRtcDate(data[3], data[4], data[5]);
                    hmi.showSavingBarAnimation(500);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

// void Settings::batteryCapacity(void)
// {
//     bool button[2] = {false,
//                       false};
//     hmi.showPage("numpad");
//     hmi.waitForPageRespon();
//     hmi.setStringToNextion("num_string.txt", String() + data.getBatteryCapacity());
//     while (!button[0] && !button[1])
//     {
//         // hmi.serialEvent_2();
//         for (uint8_t i = 0; i < 2; i++)
//             button[i] = hmi.getDataButton(i);
//     }
//     if (button[1])
//     {
//         data.setBatteryCapacity(static_cast<uint16_t>(atoi(hmi.getDataString(0))));
//         printDebugln(String() + "Battery capacity set: " + data.getBatteryCapacity() + " mAh");
//     }
// }

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
            hmi.setIntegerToNextion(String() + "b" + i + ".picc", Debug_Baud_Select);
        else
            hmi.setIntegerToNextion(String() + "b" + i + ".picc", Debug_Normal_Btn);
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
                    pointCalibration();
                    break;
                case 2:
                    resetCalibration();
                    break;
                default:
                    break;
                }
                goto start;
            }
        }
    }
}

void Settings::updateSetpointToNextion(uint8_t channel)
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

void Settings::updateAdcValueString(uint8_t channel, uint32_t *oldValue)
{
    if (channel == Channel1)
    {
        if (*oldValue != ads.adcRead[ads1][0])
        {
            *oldValue = ads.adcRead[ads1][0];
            hmi.setStringToNextion("t0.txt", ads.adcReadString[ads1][0]);
        }
    }
    else if (channel == Channel2)
    {
        if (*oldValue != ads.adcRead[ads1][1])
        {
            *oldValue = ads.adcRead[ads1][1];
            hmi.setStringToNextion("t1.txt", ads.adcReadString[ads1][1]);
        }
    }
    else if (channel == Channel3)
    {
        if (*oldValue != ads.adcRead[ads2][0])
        {
            *oldValue = ads.adcRead[ads2][0];
            hmi.setStringToNextion("t2.txt", ads.adcReadString[ads2][0]);
        }
    }
    else if (channel == Channel4)
    {
        if (*oldValue != ads.adcRead[ads2][1])
        {
            *oldValue = ads.adcRead[ads2][1];
            hmi.setStringToNextion("t3.txt", ads.adcReadString[ads2][1]);
        }
    }
    else if (channel == Channel5)
    {
        if (*oldValue != ads.adcRead[ads3][0])
        {
            *oldValue = ads.adcRead[ads3][0];
            hmi.setStringToNextion("t4.txt", ads.adcReadString[ads3][0]);
        }
    }
    else if (channel == Channel6)
    {
        if (*oldValue != ads.adcRead[ads3][1])
        {
            *oldValue = ads.adcRead[ads3][1];
            hmi.setStringToNextion("t5.txt", ads.adcReadString[ads3][1]);
        }
    }
}

void Settings::resetCalibration(void)
{
    bool button[8];
    String str = "CH1";

    hmi.showPage("rstcal");
    hmi.waitForPageRespon();

    while (true)
    {
        if (hmi.getExitPageFlag())
        {
            printDebugln("Exit Settings page");
            return;
        }
        for (int i = 0; i < 8; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 0:
                    str = "CH1";
                    break;
                case 1:
                    str = "CH2";
                    break;
                case 2:
                    str = "CH3";
                    break;
                case 3:
                    str = "CH4";
                    break;
                case 4:
                    str = "CH5";
                    break;
                case 5:
                    str = "CH6";
                    break;
                default:
                    break;
                }
                if (i < 6)
                {
                    hmi.setVisObjectNextion("q0", true);
                    hmi.setVisObjectNextion("t0", true);
                    hmi.setVisObjectNextion("b6", true);
                    hmi.setVisObjectNextion("b7", true);
                    hmi.setStringToNextion("t0.txt", str);

                    while (true)
                    {
                        if (hmi.getExitPageFlag())
                        {
                            printDebugln("Exit Settings page");
                            return;
                        }
                        if (hmi.getDataButton(6))
                        {
                            break;
                        }
                        if (hmi.getDataButton(7))
                        {
                            data.resetCalibrationData(i);
                            hmi.showSavingBarAnimation(200);
                            break;
                        }
                    }

                    hmi.setVisObjectNextion("t0", false);
                    hmi.setVisObjectNextion("b6", false);
                    hmi.setVisObjectNextion("b7", false);
                    hmi.setVisObjectNextion("q0", false);
                    hmi.flushAvailableButton();
                }
            }
        }
    }
}

void Settings::updatePointCalibParameter(uint8_t channel, uint8_t point)
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
        hmi.setIntegerToNextion("t2.picc", Pointcal_Set_Bkg);
        hmi.setIntegerToNextion("t2.picc2", Pointcal_Set_Prs_Bkg);
    }
    else
    {
        hmi.setIntegerToNextion("t2.picc", Pointcal_Normal_Bkg);
        hmi.setIntegerToNextion("t2.picc2", Pointcal_Prs_Bkg);
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
    hmi.setStringToNextion("t5.txt", utils.integerToString(data.getAdcCalibrationPoint(channelState, pointState), 10));

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
                        hmi.setIntegerToNextion("b4.picc", Pointcal_Set_Bkg);
                        hmi.setIntegerToNextion("b4.picc2", Pointcal_Set_Prs_Bkg);
                        hmi.setIntegerToNextion("q0.picc", Pointcal_Set_Bkg);
                    }
                    else if (settingState == 1)
                    {
                        settingState = 0;

                        data.setAdcCalibrationPoint(channelState, pointState, temp_adc);
                        if (((data.getAdcCalibrationPoint(channelState, pointState) > data.getAdcCalibrationPoint(channelState, pointState - 1)) && (pointState > 0 && pointState < (MAX_POINT_CAL - 1))) ||
                            ((data.getAdcCalibrationPoint(channelState, pointState) > data.getAdcCalibrationPoint(channelState, pointState - 1)) && pointState == (MAX_POINT_CAL - 1)) ||
                            ((pointState == 0) && (data.getAdcCalibrationPoint(channelState, pointState) > 0)))
                            data.setPointCalibrationStatus(channelState, pointState, true);
                        else
                            data.setPointCalibrationStatus(channelState, pointState, false);

                        hmi.showSavingBarAnimation(100);

                        hmi.setIntegerToNextion("b4.picc", Pointcal_Normal_Bkg);
                        hmi.setIntegerToNextion("b4.picc2", Pointcal_Prs_Bkg);
                        hmi.setIntegerToNextion("q0.picc", Pointcal_Normal_Bkg);
                    }
                    break;
                case 5:
                    if (data.getPointCalibrationStatus(channelState, pointState))
                    {
                        data.setPointCalibrationStatus(channelState, pointState, false);
                        hmi.showSavingBarAnimation(100);
                    }
                    break;
                default:
                    break;
                }

                updatePointCalibParameter(channelState, pointState);

                if (i != 4)
                {
                    if (settingState == 0)
                    {
                        hmi.setIntegerToNextion("b4.picc", Pointcal_Normal_Bkg);
                        hmi.setIntegerToNextion("b4.picc2", Pointcal_Prs_Bkg);
                        hmi.setIntegerToNextion("q0.picc", Pointcal_Normal_Bkg);
                    }
                }
                if (i < 4)
                {
                    hmi.setStringToNextion("t5.txt", utils.integerToString(data.getAdcCalibrationPoint(channelState, pointState), 10));
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