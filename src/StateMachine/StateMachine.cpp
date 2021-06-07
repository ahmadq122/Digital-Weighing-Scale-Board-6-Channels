#include <Arduino.h>
#include "StateMachine.h"
#include "FlashMemory/FlashMemory.h"
#include "Nextion/Nextion.h"
#include "DebugSerial/DebugSerial.h"
#include "Network/Network.h"
#include "Time/MyTime.h"
#include "MicroSD/MicroSD.h"
#include <HTTPClient.h>
#include "Settings/Settings.h"
#include "ADC/ADS1232.h"
#include "RTOS/RTOS.h"
#include "Datalogger/Datalogger.h"
#include "PinMap.h"
#include "PictureListID.h"

/*****************************************************************************
 *******************************ESP32S-PIN************************************
 *                  ---------------------------------
 *              ---|3.3V     ---__---__-----      GND|---
 *              ---|EN/RST  |            | |       23|---VSPI_MOSI
 *            AI---|36       _______________       22|---SCL
 *            AI---|39      |               |       1|---TX0
 *            AI---|34      | ESP-WROOM-32  |       3|---RX0
 *            AI---|35      |               |      21|---SDA
 *           DIO---|32      |               |     GND|---
 *           DIO---|33      |               |      19|---VSPI_MISO
 *           DIO---|25      |_______________|      18|---VSPI_SCK
 *           DIO---|26                              5|---DIO
 *           DIO---|27                             17|---TX2
 *           DIO---|14                             16|---RX2
 *           DIO---|12                             -4|---DIO
 *              ---|GND                            -0|---DIO
 *           DIO---|13                              2|---DIO
 *           RX1---|9                              15|---DIO
 *           TX1---|10           _______            8|---SD1*
 *      *CSC/CMD---|11   ___    |       |    ____   7|---SD0*
 *              ---|Vin |EN |   |       |   |BOOT|  6|---SCK/CLK*
 *                  ------------|_______|------------
 * 
 *          *Pins are connected to the integrated SPI Flash integrated on
 *           ESP-32 and are not recommended for other uses.
******************************************************************************/

// 36,//can't be used as output pin
// 39,////can't be used as output pin
// 34,//can't be used as output pin
// 35,//can't be used as output pin
// 9,
// 10,
// 11,6,7,8
// uint8_t pin[10] = {
//     12,
//     13,
//     15,
//     2,
//     32,
//     33,
//     25,
//     26,
//     27,
//     14};

void StateMachine::setup(void)
{
    bool flashBegin = false;
    uint8_t counter = 0;
    pinMode(Pin_Buzzer, OUTPUT);
    digitalWrite(Pin_Buzzer, 0);

    delay(100);

    Serial.begin(115200);
    NexSerial.begin(115200);
    while (!NexSerial)
        ;
    hmi.showPage("start");
    //waiting for Nextion page ready
    while (!hmi.getWaitingEndSignal())
    {
        hmi.serialEvent_2();
        delay(100);
        if (++counter > 10)
        {
            hmi.showPage("start"); //call back start page if still no respons from LCD
            counter = 0;
        }
    }
    Serial.end();

    rtos.updateStartProgressBar(10);
    while (flashBegin == false)
    {
        flashBegin = initFlash(MEMORY_SIZE);
        delay(200);
    }
    delay(10);

    rtos.updateStartProgressBar(10);
    if (data.getDebugMode())
    {
        Serial.begin(data.getBaudrateSerial(debugging));
        while (!Serial)
            ;
    }
    Serial1.begin(115200);
    while (!Serial1)
        ;

    rtos.updateStartProgressBar(10);
    ads.begin();
    rtos.setup();
    net.setup();
    initTime();
    initSDCard();
    rtos.updateStartProgressBar(10);
    hmi.init();

    while (rtos.startProgressBar < 100)
    {
        printDebugln(String() + "Progress: " + rtos.startProgressBar + " %");
        rtos.updateStartProgressBar(5);
        delay(50);
    }
    digitalWrite(Pin_Buzzer, 1);
    delay(100);
    digitalWrite(Pin_Buzzer, 0);
}

bool StateMachine::initTime(void)
{
    bool ret = RTC.begin();
    mtime.initOffset(data.getTimezone());
    return ret;
}

bool StateMachine::initFlash(uint16_t memory)
{
    if (memory > MEMORY_SIZE)
        return false;
    return data.begin(memory);
}

void StateMachine::initSDCard(void)
{
    card.setup();
}

/******************
 * List of Button at HomeScreen
 * 0) Network
 * 1) Settings
 * 2) Measurement Units
 * 3) Data logger
 * 4) EN/DIS Channel 1
 * 5) EN/DIS Channel 2
 * 6) EN/DIS Channel 3
 * 7) EN/DIS Channel 4
 * 8) EN/DIS Channel 5
 * 9) EN/DIS Channel 6
 * 10) Tare Channel 1
 * 11) Tare Channel 2
 * 12) Tare Channel 3
 * 13) Tare Channel 4
 * 14) Tare Channel 5
 * 15) Tare Channel 6
*/

uint8_t StateMachine::homeScreen(void)
{
    uint8_t button[16] = {0};
    bool enDisChannel[MAX_CHANNEL];
    bool sClock = false;
    bool sClockUpdate = false;
    uint8_t wifiSignal = 0;
    String tempString;
    char timeString[10];
    char timeNDateString[20];

    hmi.showPage("home");
    hmi.waitForPageRespon();

    for (uint8_t i = 0; i < MAX_CHANNEL; i++)
    {
        enDisChannel[i] = data.getChannelEnDisStatus(i);
        updateButtonToggleStateToNextion(i);
        hmi.setStringToNextion((String() + "t_ch" + (i + 1) + ".txt"), "00000.00");
        hmi.setStringToNextion(String() + "t_mu" + (i + 1) + ".txt", getStringUnit(data.getMeasurementUnit()));
    }
    //initialize the signal level base on the last signal value
    hmi.setIntegerToNextion("signal.val", signalValue);
    hmi.setIntegerToNextion("bat.val", batteryValue);

    while (true)
    {
        mtime.updateRTC_N_NTPTime();

        if (!sClockUpdate)
        {
            if (rtos.interruptSeconds == 0)
            {
                sClock = !sClock;
                hmi.setIntegerToNextion("sClock.en", sClock);
                hmi.setVisObjectNextion("clock", !sClock);
                if (sClock)
                    printDebugln("Scrool text enabled");
                else
                    printDebugln("Scrool text disabled");
                sClockUpdate = true;
            }
        }
        if (rtos.interruptSeconds != 0)
            sClockUpdate = false;

        if (!sClock)
        {
            if (rtos.secondTriggered[0])
            {
                mtime.getTimeStr(timeString);
                // Serial.println(timeString);
                hmi.setStringToNextion("clock.txt", timeString);
                rtos.secondTriggered[0] = 0;
            }
        }
        else
        {
            if (rtos.secondTriggered[1])
            {
                mtime.getTimeAndDateStr(timeNDateString);
                // tempString = String() + "00:" + interruptSeconds + " 18/03/2021";
                // Serial.println(timeNDateString);
                hmi.setStringToNextion("sClock.txt", timeNDateString);
                rtos.secondTriggered[1] = 0;
            }
        }

        for (int i = 0; i < 16; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                if (button[0])
                {
                    return NETWORK;
                }
                else if (button[1])
                {
                    return SETTINGS;
                }
                else if (button[2])
                {
                    return UNITS;
                }
                else if (button[3])
                {
                    return DATALOG;
                }
                else if (button[4])
                {
                    enDisChannel[Channel1] = !enDisChannel[Channel1];
                    data.setChannelEnDisStatus(Channel1, enDisChannel[Channel1]);
                    updateButtonToggleStateToNextion(Channel1);
                    if (enDisChannel[Channel1])
                        printDebugln("Channel 1 : Enabled");
                    else
                        printDebugln("Channel 1 : Disabled");
                }
                else if (button[5])
                {
                    enDisChannel[Channel2] = !enDisChannel[Channel2];
                    data.setChannelEnDisStatus(Channel2, enDisChannel[Channel2]);
                    updateButtonToggleStateToNextion(Channel2);
                    if (enDisChannel[Channel2])
                        printDebugln("Channel 2 : Enabled");
                    else
                        printDebugln("Channel 2 : Disabled");
                }
                else if (button[6])
                {
                    enDisChannel[Channel3] = !enDisChannel[Channel3];
                    data.setChannelEnDisStatus(Channel3, enDisChannel[Channel3]);
                    updateButtonToggleStateToNextion(Channel3);
                    if (enDisChannel[Channel3])
                        printDebugln("Channel 3 : Enabled");
                    else
                        printDebugln("Channel 3 : Disabled");
                }
                else if (button[7])
                {
                    enDisChannel[Channel4] = !enDisChannel[Channel4];
                    data.setChannelEnDisStatus(Channel4, enDisChannel[Channel4]);
                    updateButtonToggleStateToNextion(Channel4);
                    if (enDisChannel[Channel4])
                        printDebugln("Channel 4 : Enabled");
                    else
                        printDebugln("Channel 4 : Disabled");
                }
                else if (button[8])
                {
                    enDisChannel[Channel5] = !enDisChannel[Channel5];
                    data.setChannelEnDisStatus(Channel5, enDisChannel[Channel5]);
                    updateButtonToggleStateToNextion(Channel5);
                    if (enDisChannel[Channel5])
                        printDebugln("Channel 5 : Enabled");
                    else
                        printDebugln("Channel 5 : Disabled");
                }
                else if (button[9])
                {
                    enDisChannel[Channel6] = !enDisChannel[Channel6];
                    data.setChannelEnDisStatus(Channel6, enDisChannel[Channel6]);
                    updateButtonToggleStateToNextion(Channel6);
                    if (enDisChannel[Channel6])
                        printDebugln("Channel 6 : Enabled");
                    else
                        printDebugln("Channel 6 : Disabled");
                }
                else if (button[10])
                {
                    ads.enableTare[Channel1] = true;
                    printDebugln("Channel 1 Tare >>>>>>>>>>>>>>>>>>>");
                    // while (ads.enableTare[Channel1])
                    //     ;
                    hmi.showSavingBarAnimation(100);
                }
                else if (button[11])
                {
                    ads.enableTare[Channel2] = true;
                    printDebugln("Channel 2 Tare >>>>>>>>>>>>>>>>>>>");
                    // while (ads.enableTare[Channel2])
                    //     ;
                    hmi.showSavingBarAnimation(100);
                }
                else if (button[12])
                {
                    ads.enableTare[Channel3] = true;
                    printDebugln("Channel 3 Tare >>>>>>>>>>>>>>>>>>>");
                    // while (ads.enableTare[Channel3])
                    //     ;
                    hmi.showSavingBarAnimation(100);
                }
                else if (button[13])
                {
                    ads.enableTare[Channel4] = true;
                    printDebugln("Channel 4 Tare >>>>>>>>>>>>>>>>>>>");
                    // while (ads.enableTare[Channel4])
                    //     ;
                    hmi.showSavingBarAnimation(100);
                }
                else if (button[14])
                {
                    ads.enableTare[Channel5] = true;
                    printDebugln("Channel 5 Tare >>>>>>>>>>>>>>>>>>>");
                    // while (ads.enableTare[Channel5])
                    //     ;
                    hmi.showSavingBarAnimation(100);
                }
                else if (button[15])
                {
                    ads.enableTare[Channel6] = true;
                    printDebugln("Channel 6 Tare >>>>>>>>>>>>>>>>>>>");
                    // while (ads.enableTare[Channel6])
                    //     ;
                    hmi.showSavingBarAnimation(100);
                }
            }
        }
        updateWeightStringToNextion();
        updateExceedMaximumFlagToNextion();
        if (net.checkConnection(&wifiSignal, &rtos.wifiConnectionTriggered))
        {
            rtos.wifiConnected = true;
            updateSignalIndicatorToNextion(wifiSignal);
        }
        else
        {
            updateSignalIndicatorToNextion(0);
            rtos.wifiConnected = false;
        }
        updateBatteryIndicatorToNextion(getBatteryPercent());
        evenBuzzer();

        if (rtos.secondTriggered[5])
        {
            for (uint8_t i = 0; i < 3; i++)
            {
                if (data.getDatalogStatus(i))
                {
                    if (!dataLoggingState[i])
                    {
                        if (logger.checkSchedule(_on_, i))
                            dataLoggingState[i] = true;
                    }
                    else
                    {
                        logger.logData(i);
                        if (logger.checkSchedule(_off_, i))
                            dataLoggingState[i] = false;
                    }
                }
            }
            rtos.secondTriggered[5] = 0;
        }
    }
    return 0;
}
uint8_t StateMachine::networkSettings(void)
{
    while (true)
    {
        if (net.networkConfig())
        {
            net.networkScanning();
        }
        else
            break;
    }
    return 0;
}
uint8_t StateMachine::settings(void)
{
    setting.mainMenu();
    return 0;
}
uint8_t StateMachine::measurementUnits(void)
{
    uint8_t button;

    hmi.showPage("meaunit");
    // hmi.setIntegerToNextion("unit.val", data.getMeasurementUnit());
    hmi.waitForPageRespon();
    printDebugln("Measurement Unit page opened");

    updateSelectedUnitToNextion(data.getMeasurementUnit());
    while (true)
    {
        while (!hmi.checkAnyButtonPressed(&button))
        {
            // hmi.serialEvent_2();
        }
        if (hmi.getExitPageFlag())
        {
            break;
        }
        for (uint8_t i = 0; i < 10; i++)
        {
            if (hmi.getDataButton(i))
            {
                if (data.setMeasurementUnit(i))
                {
                    updateSelectedUnitToNextion(data.getMeasurementUnit());
                    // hmi.setIntegerToNextion("unit.val", data.getMeasurementUnit());
                }
            }
        }
    }
    return 0;
}
uint8_t StateMachine::datalogSettings(void)
{
__start:
    hmi.showPage("datalog");
    hmi.waitForPageRespon();
    printDebugln("Datalog page opened");
    while (!hmi.getExitPageFlag())
    {
        if (hmi.getDataButton(0))
        {
            logger.setting(serial);
            goto __start;
        }
        else if (hmi.getDataButton(1))
        {
            logger.setting(local);
            goto __start;
        }
        else if (hmi.getDataButton(2))
        {
            logger.setting(remote);
            goto __start;
        }
    }
    return 0;
}

/////////Private function
void StateMachine::updateButtonToggleStateToNextion(uint8_t channel)
{
    String bt_enVariable[MAX_CHANNEL] = {
        "bt_en1.val",
        "bt_en2.val",
        "bt_en3.val",
        "bt_en4.val",
        "bt_en5.val",
        "bt_en6.val"};
    hmi.setIntegerToNextion(bt_enVariable[channel].c_str(), data.getChannelEnDisStatus(channel));
}

void StateMachine::updateWeightStringToNextion(void)
{
    String newWeightString[MAX_CHANNEL];

    for (uint8_t i = 0; i < MAX_CHANNEL; i++)
    {
        if (data.getChannelEnDisStatus(i))
        {
            newWeightString[i] = ads.getStringWeightInUnit(i);
            if (prevWeightString[i] != newWeightString[i])
            {
                hmi.setStringToNextion((String() + "t_ch" + (i + 1) + ".txt"), newWeightString[i]);
                printDebugln(String() + "Updated: " + " Channel " + (i + 1) + " weight");
            }
            prevWeightString[i] = newWeightString[i];
        }
    }
}

void StateMachine::evenBuzzer(void)
{
    if (!data.getBuzzerMute())
    {
        if (hmi.getDataBuzzer() == 2)
        {
            if (!buzzerState)
            {
                buzzerState = true;
                digitalWrite(Pin_Buzzer, 1);
            }
        }
        else if (hmi.getDataBuzzer() == 1)
        {
            if (buzzerState)
            {
                buzzerState = false;
                digitalWrite(Pin_Buzzer, 0);
            }
        }
        else
        {
            digitalWrite(Pin_Buzzer, 0);
        }
    }
    else
        digitalWrite(Pin_Buzzer, 0);
}

void StateMachine::updateSignalIndicatorToNextion(uint8_t newValue)
{
    if (newValue != signalValue)
    {
        hmi.setIntegerToNextion("signal.val", newValue);
        signalValue = newValue;
        printDebugln(String() + "Updated: " + newValue + "%" + " signal");
    }
}
void StateMachine::updateBatteryIndicatorToNextion(uint8_t newValue)
{
    if (newValue != batteryValue)
    {
        hmi.setIntegerToNextion("bat.val", newValue);
        batteryValue = newValue;
        printDebugln(String() + "Updated: " + newValue + "%" + " battery");
    }
}

uint8_t StateMachine::getBatteryPercent(void)
{
    float adcBattery = 0;
    uint8_t percent = 0;
    static uint8_t index = 0;

    //USE MOVING AVERAGE METHOD

    if (index < ADCsample)
    {
        adcBatteryContainer[index] = analogRead(Pin_VBat_Sense);
        index++;
    }
    else
    {
        for (uint8_t i = 0; i < (ADCsample - 1); i++)
        {
            adcBatteryContainer[i] = adcBatteryContainer[i + 1];
        }
        adcBatteryContainer[ADCsample - 1] = analogRead(Pin_VBat_Sense);
        for (uint8_t i = 0; i < ADCsample; i++)
        {
            adcBattery += adcBatteryContainer[i];
        }
        adcBattery /= ADCsample;
        percent = (adcBattery / 4095) * 100;
        return percent;
    }
    return 0;
}

String StateMachine::getStringUnit(uint8_t unit)
{
    if (unit == gram)
        return "[g]";
    else if (unit == milligram)
        return "[mg]";
    else if (unit == pound)
        return "[lb]";
    else if (unit == ounce)
        return "[oz]";
    else if (unit == troy_ounce)
        return "[ozt]";
    else if (unit == carat)
        return "[ct]";
    else if (unit == kilogram)
        return "[kg]";
    else if (unit == newton)
        return "[N]";
    else if (unit == dram)
        return "[d]";
    else if (unit == grain)
        return "[GN]";
    else
        return "[g]";
}

bool StateMachine::isWeightExceedMaximumValue(uint8_t channel, float actualWeight)
{
    if (data.getGramMaximum(channel) != data.getGramMaximum(channel))
    { //check if float is nan value
        return false;
    }
    return ((actualWeight > (data.getGramMaximum(channel) / ads.dividerUnits[data.getMeasurementUnit()])) && (actualWeight != ERROR_WEIGHT));
}

void StateMachine::updateExceedMaximumFlagToNextion(void)
{
    // float actual[MAX_CHANNEL];
    uint32_t temp;

    // for (uint8_t i = 0; i < MAX_CHANNEL; i++)
    // {
    //     if (data.getChannelEnDisStatus(i))
    //         actual[i] = ads.getWeightInUnit(i);
    // }

    if (rtos.secondTriggered[2])
    {
        for (uint8_t i = 0; i < MAX_CHANNEL; i++)
        {
            if (data.getChannelEnDisStatus(i))
            {
                if (isWeightExceedMaximumValue(i, ads.getWeightInUnit(i)))
                {
                    hmi.setIntegerToNextion((String() + "max" + (i + 1) + ".val"), 1);
                    maxState[i] = true;
                }
                else
                {
                    if (maxState[i] == true)
                    {
                        hmi.getValue((String() + "max" + (i + 1) + ".val").c_str(), &temp);
                        // Serial.println(String() + "max" + (i + 1) + ":" + temp);
                        if (temp == 1)
                        {
                            hmi.setIntegerToNextion((String() + "max" + (i + 1) + ".val"), 0);
                            maxState[i] = false;
                        }
                    }
                }
            }
        }
        rtos.secondTriggered[2] = 0;
    }
}

void StateMachine::updateSelectedUnitToNextion(uint8_t unit)
{
    for (uint8_t i = 0; i < 10; i++)
    {
        hmi.setIntegerToNextion(String() + "b" + i + ".picc", unit == i ? Measurement_List_Select : Measurement_List_Bkg);
    }
}
