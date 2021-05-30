#include "FlashMemory.h"
#include "EEPROM.h"
#include "../DebugSerial/DebugSerial.h"
#include "RTOS/RTOS.h"
#include "Utility/Utility.h"
#include "PinMap.h"

bool MemoryFlash::begin(uint16_t sizeOfMemory)
{
    //init setup flash memory to use 512 byte storage
    Serial.begin(115200);
    if (EEPROM.begin(sizeOfMemory))
    {
        readAll();
        printDebugln("Initialization of flash succesful");
        return true;
    }
    else
    {
        printDebugln("Initialization of flash Failed!");
    }
    Serial.end();
    return false;
}

bool MemoryFlash::flushFlash()
{
    byte byteData = 0;
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        EEPROM.write(i, 0xFF);
    }
    EEPROM.commit();
    delay(10);
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        byteData = EEPROM.read(i);
        if (byteData != 0xFF)
            return false;
    }
    return true;
}

bool MemoryFlash::resetDefault()
{
    if (flushFlash())
    {
        strcpy(flash.ssid, "");
        strcpy(flash.password, "");
        strcpy(flash.keyAPI, "");
        flash.encryptType = 0;
        flash.timeZone = 0;        //WIB
        flash.measurementUnit = 0; //GRAMS
        flash.channelEnDisStatus = B00111111;
        flash.generalStatus = B00000001;

        for (uint8_t i = 0; i < MAX_CHANNEL; i++)
        {
            // flash.fieldChannel[i] = i;
            flash.pointCalibrationStatus[i] = 0;
            flash.gramMaximum[i] = 0;
            for (uint8_t j = 0; j < MAX_POINT_CAL; j++)
                flash.adcCalibrationPoint[i][j] = 0;
            for (uint8_t k = 0; k < MAX_POINT_CAL - 1; k++)
                flash.gramCalibrationPoint[i][k] = 0.0;
        }
        // flash.batteryCapacity = 1000;
        flash.periodDataLog[local] = 1;
        flash.periodDataLog[serial] = 1;
        flash.periodDataLog[remote] = 180;
        flash.baudrateSerial[debugging] = 6;
        flash.baudrateSerial[logging] = 6;

        flash.timeSchedulerDataLog[_off_][serial][0] = 0;
        flash.timeSchedulerDataLog[_on_][serial][0] = 0;
        flash.timeSchedulerDataLog[_off_][local][0] = 0;
        flash.timeSchedulerDataLog[_on_][local][0] = 0;
        flash.timeSchedulerDataLog[_off_][remote][0] = 0;
        flash.timeSchedulerDataLog[_on_][remote][0] = 0;

        flash.timeSchedulerDataLog[_off_][serial][1] = 0;
        flash.timeSchedulerDataLog[_on_][serial][1] = 0;
        flash.timeSchedulerDataLog[_off_][local][1] = 0;
        flash.timeSchedulerDataLog[_on_][local][1] = 0;
        flash.timeSchedulerDataLog[_off_][remote][1] = 0;
        flash.timeSchedulerDataLog[_on_][remote][1] = 0;

        flash.dateSchedulerDataLog[_off_][serial] = 0;
        flash.dateSchedulerDataLog[_on_][serial] = 0;
        flash.dateSchedulerDataLog[_off_][local] = 0;
        flash.dateSchedulerDataLog[_on_][local] = 0;
        flash.dateSchedulerDataLog[_off_][remote] = 0;
        flash.dateSchedulerDataLog[_on_][remote] = 0;

        flash.enableDateScheduler = B00000000;
        flash.dimScreenTimer = 0;
        flash.screenBrightness = 100;
        storeDataToFlash();
    }
    else
        return false;

    return true;
}

bool MemoryFlash::resetCalibrationData(uint8_t channel)
{
    flash.pointCalibrationStatus[channel] = 0;
    for (uint8_t j = 0; j < MAX_POINT_CAL; j++)
        flash.adcCalibrationPoint[channel][j] = 0;
    for (uint8_t k = 0; k < MAX_POINT_CAL - 1; k++)
        flash.gramCalibrationPoint[channel][k] = 0.0;

    return storeDataToFlash();
}

void MemoryFlash::readAll()
{

    EEPROM.get(0, flash);
    printDebugln("All flash data read!");
}

////update data struct flash to flash memory
bool MemoryFlash::storeDataToFlash(void)
{
    EEPROM.put(0, flash);
    if (EEPROM.commit())
    {
        printDebugln("Stored succesfully!");
        return true;
    }
    else
    {
        printDebugln("failed to stored!!!");
        return false;
    }
}

////setting data flash
bool MemoryFlash::setSSID(const char *newValue)
{
    // String _ssid = newValue;
    uint8_t size = utils.charArraySize(newValue);

    if (strcmp(newValue, "") == 0)
    {
        printDebugln(String() + "Data Failed to be stored!");
        return false;
    }

    if (strcmp(newValue, flash.ssid) && size < MAX_SSID_CHAR) //if data is different
    {
        strcpy(flash.ssid, newValue);
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        if (size >= MAX_SSID_CHAR)
            printDebugln(String() + "SSID char length " + size + " exceeds the maximum limit " + (MAX_SSID_CHAR - 1));
        else
            printDebugln(String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setPassword(const char *newValue)
{
    // String _password = newValue;
    uint8_t size = utils.charArraySize(newValue);

    if (strcmp(newValue, "") == 0)
    {
        printDebugln(String() + "Data Failed to be stored!");
        return false;
    }
    if (strcmp(newValue, flash.password) && size < MAX_PASSWORD_CHAR) //if data is different
    {
        strcpy(flash.password, newValue);
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        if (size >= MAX_PASSWORD_CHAR)
            printDebugln(String() + "Password char length " + size + " exceeds the maximum limit " + (MAX_PASSWORD_CHAR - 1));
        else
            printDebugln(String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setKeyAPI(const char *newValue)
{
    // String _apiKey = newValue;
    uint8_t size = utils.charArraySize(newValue);

    if (strcmp(newValue, "") == 0)
    {
        printDebugln(String() + "Data Failed to be stored!");
        return false;
    }
    if (strcmp(newValue, flash.keyAPI) && size < MAX_APIKEY_CHAR) //if data is different
    {
        strcpy(flash.keyAPI, newValue);
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        if (size >= MAX_APIKEY_CHAR)
            printDebugln(String() + "API Key char length " + size + " exceeds the maximum limit " + (MAX_APIKEY_CHAR - 1));
        else
            printDebugln(String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setEncryptType(uint8_t newValue)
{
    if (newValue != flash.encryptType)
    {
        flash.encryptType = newValue;
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setTimezone(uint8_t newValue)
{
    if (newValue != flash.timeZone)
    {
        flash.timeZone = newValue;
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setMeasurementUnit(uint8_t newValue)
{
    if (newValue != flash.measurementUnit)
    {
        flash.measurementUnit = newValue;
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setChannelEnDisStatus(uint8_t channel, bool newValue)
{
    if (newValue != bitRead(flash.channelEnDisStatus, channel))
    {
        if (newValue)
        {
            bitSet(flash.channelEnDisStatus, channel);
        }
        else
        {
            bitClear(flash.channelEnDisStatus, channel);
        }
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setDebugMode(bool newValue)
{
    if (newValue != bitRead(flash.generalStatus, DEBUG_ENABLE))
    {
        if (newValue)
            bitSet(flash.generalStatus, DEBUG_ENABLE);
        else
            bitClear(flash.generalStatus, DEBUG_ENABLE);
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setNetworkEnable(bool newValue)
{
    if (newValue != bitRead(flash.generalStatus, NETWORK_ENABLE))
    {
        if (newValue)
            bitSet(flash.generalStatus, NETWORK_ENABLE);
        else
            bitClear(flash.generalStatus, NETWORK_ENABLE);
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setDatalogStatus(uint8_t loggerType, bool newValue)
{
    uint8_t type = 0;

    if (loggerType > remote)
        return false;

    if (loggerType == serial)
        type = SERIAL_LOG_ENABLE;
    else if (loggerType == local)
        type = LOCAL_LOG_ENABLE;
    else if (loggerType == remote)
        type = WEB_LOG_ENABLE;

    if (newValue != bitRead(flash.generalStatus, type))
    {
        if (newValue)
            bitSet(flash.generalStatus, type);
        else
            bitClear(flash.generalStatus, type);
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}

bool MemoryFlash::setSpeedRate(bool newValue)
{
    if (newValue != bitRead(flash.generalStatus, SPEED_RATE))
    {
        if (newValue)
        {
            bitSet(flash.generalStatus, SPEED_RATE);
            digitalWrite(Pin_Speedrate, HIGH);
        }
        else
        {
            bitClear(flash.generalStatus, SPEED_RATE);
            digitalWrite(Pin_Speedrate, LOW);
        }
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}

// bool MemoryFlash::setBatteryCapacity(uint16_t newValue)
// {
//     if (newValue != flash.batteryCapacity)
//     {
//         flash.batteryCapacity = newValue;
//         storeDataToFlash();
//         printDebugln(String() + "New data set : " + newValue);
//         return true;
//     }
//     else
//     {
//         printDebugln(String() + "New data already in flash!");
//     }
//     return false;
// }

// bool MemoryFlash::setFieldChannel(uint8_t channel, uint8_t newValue)
// {
//     if (newValue != flash.fieldChannel[channel])
//     {
//         flash.fieldChannel[channel] = newValue;
//         storeDataToFlash();
//         printDebugln(String() + "New data set : " + newValue);
//         return true;
//     }
//     else
//     {
//         printDebugln(String() + "New data already in flash!");
//     }
//     return false;
// }
bool MemoryFlash::setPointCalibrationStatus(uint8_t channel, uint8_t point, bool newValue)
{
    if (newValue != bitRead(flash.pointCalibrationStatus[channel], point))
    {
        if (newValue)
            bitSet(flash.pointCalibrationStatus[channel], point);
        else
            bitClear(flash.pointCalibrationStatus[channel], point);
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setPeriodDatalog(uint8_t loggerType, uint32_t newValue)
{
    if (loggerType > remote)
        return false;
    if (newValue != flash.periodDataLog[loggerType])
    {
        flash.periodDataLog[loggerType] = newValue;
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}

bool MemoryFlash::setBaudrateSerial(bool type, uint8_t newValue)
{
    if (newValue != flash.baudrateSerial[type])
    {
        flash.baudrateSerial[type] = newValue;
        storeDataToFlash();
        printDebugln(String() + "New data set : " + baudrate[newValue]);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setAdcCalibrationPoint(uint8_t channel, uint8_t point, uint32_t newValue)
{
    if (newValue != flash.adcCalibrationPoint[channel][point])
    {
        flash.adcCalibrationPoint[channel][point] = newValue;
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setGramCalibrationPoint(uint8_t channel, uint8_t point, float newValue)
{
    if (newValue != flash.gramCalibrationPoint[channel][point])
    {
        flash.gramCalibrationPoint[channel][point] = newValue;
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}

bool MemoryFlash::setGramMaximum(uint8_t channel, float newValue)
{
    if (newValue != flash.gramMaximum[channel])
    {
        flash.gramMaximum[channel] = newValue;
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setTimeSchedulerDatalog(bool scheduleType, uint8_t loggerType, uint8_t index, uint16_t newValue)
{
    uint16_t enable = 0x1000 & flash.timeSchedulerDataLog[scheduleType][loggerType][index];
    uint16_t oldValue = 0x0FFF & flash.timeSchedulerDataLog[scheduleType][loggerType][index];
    newValue = newValue & 0x0FFF;

    if (newValue != oldValue)
    {
        flash.timeSchedulerDataLog[scheduleType][loggerType][index] = newValue | enable;

        storeDataToFlash();
        printDebugln(String() + "New data set (HH:MM): " + (newValue / 60) + ":" + (newValue % 60));
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}

bool MemoryFlash::setEnableTimeScheduler(bool scheduleType, uint8_t loggerType, uint8_t index, bool newValue)
{
    uint16_t schedule = 0x0FFF & flash.timeSchedulerDataLog[scheduleType][loggerType][index];
    bool oldValue = ((0x1000 & flash.timeSchedulerDataLog[scheduleType][loggerType][index]) > 0) ? true : false;

    if (newValue != oldValue)
    {
        if (newValue)
        {
            flash.timeSchedulerDataLog[scheduleType][loggerType][index] = schedule | 0x1000;
        }
        else
        {
            flash.timeSchedulerDataLog[scheduleType][loggerType][index] = schedule & 0xEFFF;
        }

        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}

/*
* Date  : 01 - 31, maximum takes 5 bits //0xF800
* Month : 01 - 12, maximum takes 4 bits //0x0780
* Year  : 00 - 99, maximum takes 7 bits //0x007F
* total bit : 16 bits 
*/
bool MemoryFlash::setDateSchedulerDatalog(bool scheduleType, uint8_t loggerType, uint8_t newDate, uint8_t newMonth, uint8_t newYear)
{
    uint16_t oldValue = flash.dateSchedulerDataLog[scheduleType][loggerType];
    uint16_t newValue = ((static_cast<uint16_t>(newDate) << 11) & 0xF800) | ((static_cast<uint16_t>(newMonth) << 7) & 0x0780);
    newValue |= (static_cast<uint16_t>(newYear) & 0x007F);

    if (newValue != oldValue)
    {
        flash.dateSchedulerDataLog[scheduleType][loggerType] = newValue;

        storeDataToFlash();
        printDebugln(String() + "New data set :" + newDate + "/" + newMonth + "/" + newYear);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}

bool MemoryFlash::setEnableDateScheduler(bool scheduleType, uint8_t loggerType, bool newValue)
{
    uint8_t index = 0;
    bool oldValue = false;
    if (loggerType == serial)
    {
        index = (scheduleType == _off_) ? 0 : 1;
    }
    else if (loggerType == local)
    {
        index = (scheduleType == _off_) ? 2 : 3;
    }
    else if (loggerType == remote)
    {
        index = (scheduleType == _off_) ? 4 : 5;
    }

    oldValue = bitRead(flash.enableDateScheduler, index);

    if (newValue != oldValue)
    {
        if (newValue)
        {
            bitSet(flash.enableDateScheduler, index);
        }
        else
        {
            bitClear(flash.enableDateScheduler, index);
        }

        storeDataToFlash();
        printDebugln(String() + "new data " + newValue + " set at index " + index);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}

bool MemoryFlash::setDimScreenTimer(uint8_t newValue)
{
    if (newValue != flash.dimScreenTimer)
    {
        flash.dimScreenTimer = newValue;
        storeDataToFlash();
        printDebugln(String() + "New data set : " + dim[newValue]);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setScreenBrightness(uint8_t newValue)
{
    if (newValue != flash.screenBrightness)
    {
        flash.screenBrightness = newValue;
        storeDataToFlash();
        printDebugln(String() + "New data set : " + newValue);
        return true;
    }
    else
    {
        printDebugln(String() + "New data already in flash!");
    }
    return false;
}

////reading data flash
const char *MemoryFlash::getSSID(void)
{
    return flash.ssid;
}
const char *MemoryFlash::getPassword(void)
{
    return flash.password;
}
const char *MemoryFlash::getKeyAPI(void)
{
    return flash.keyAPI;
}
uint8_t MemoryFlash::getEncryptType(void)
{
    return flash.encryptType;
}
uint8_t MemoryFlash::getTimezone(void)
{
    return flash.timeZone;
}
uint8_t MemoryFlash::getMeasurementUnit(void)
{
    return flash.measurementUnit;
}
bool MemoryFlash::getChannelEnDisStatus(uint8_t channel)
{
    return bitRead(flash.channelEnDisStatus, channel);
}
bool MemoryFlash::getDebugMode(void)
{
    return bitRead(flash.generalStatus, DEBUG_ENABLE);
}
bool MemoryFlash::getNetworkEnable(void)
{
    return bitRead(flash.generalStatus, NETWORK_ENABLE);
}
bool MemoryFlash::getDatalogStatus(uint8_t loggerType)
{
    uint8_t type = serial;
    if (loggerType > remote)
        return false;

    if (loggerType == local)
        type = LOCAL_LOG_ENABLE;
    else if (loggerType == serial)
        type = SERIAL_LOG_ENABLE;
    else if (loggerType == remote)
        type = WEB_LOG_ENABLE;

    return bitRead(flash.generalStatus, type);
}
bool MemoryFlash::getSpeedRate(void)
{
    return bitRead(flash.generalStatus, SPEED_RATE);
}
// uint8_t MemoryFlash::getFieldChannel(uint8_t channel)
// {
//     return flash.fieldChannel[channel];
// }
bool MemoryFlash::getPointCalibrationStatus(uint8_t channel, uint8_t point)
{
    return bitRead(flash.pointCalibrationStatus[channel], point);
}
uint8_t MemoryFlash::getPointCalibrationStatus(uint8_t channel)
{
    return flash.pointCalibrationStatus[channel];
}
// uint16_t MemoryFlash::getBatteryCapacity(void)
// {
//     return flash.batteryCapacity;
// }
uint32_t MemoryFlash::getPeriodDatalog(uint8_t loggerType)
{
    if (loggerType > remote)
        return 0;
    return flash.periodDataLog[loggerType];
}

uint32_t MemoryFlash::getBaudrateSerial(bool type)
{
    return baudrate[flash.baudrateSerial[type]];
}
uint8_t MemoryFlash::getBaudrateSerialIndex(bool type)
{
    return flash.baudrateSerial[type];
}
uint32_t MemoryFlash::getAdcCalibrationPoint(uint8_t channel, uint8_t point)
{
    return flash.adcCalibrationPoint[channel][point];
}
float MemoryFlash::getGramCalibrationPoint(uint8_t channel, uint8_t point)
{
    return flash.gramCalibrationPoint[channel][point];
}
float MemoryFlash::getGramMaximum(uint8_t channel)
{
    return flash.gramMaximum[channel];
}

void MemoryFlash::getTimeSchedulerDatalog(bool scheduleType, uint8_t loggerType, uint8_t index, uint8_t *hour, uint8_t *minute)
{
    uint16_t value = flash.timeSchedulerDataLog[scheduleType][loggerType][index] & 0x0FFF;
    *hour = static_cast<uint8_t>(value / 60);
    *minute = static_cast<uint8_t>(value % 60);
}

bool MemoryFlash::getEnableTimeScheduler(bool scheduleType, uint8_t loggerType, uint8_t index)
{
    return ((0x1000 & flash.timeSchedulerDataLog[scheduleType][loggerType][index]) > 0) ? true : false;
}

void MemoryFlash::getDateSchedulerDatalog(bool scheduleType, uint8_t loggerType, uint8_t *date, uint8_t *month, uint8_t *year)
{
    uint16_t value = flash.dateSchedulerDataLog[scheduleType][loggerType];
    *date = static_cast<uint8_t>((value & 0xF800) >> 11);
    *month = static_cast<uint8_t>((value & 0x0780) >> 7);
    *year = static_cast<uint8_t>(value & 0x007F);
}

bool MemoryFlash::getEnableDateScheduler(bool scheduleType, uint8_t loggerType)
{
    uint8_t index = 0;

    if (loggerType == serial)
    {
        index = (scheduleType == _off_) ? 0 : 1;
    }
    else if (loggerType == local)
    {
        index = (scheduleType == _off_) ? 2 : 3;
    }
    else if (loggerType == remote)
    {
        index = (scheduleType == _off_) ? 4 : 5;
    }

    return (bitRead(flash.enableDateScheduler, index));
}

uint16_t MemoryFlash::getDimScreenTimer(void)
{
    return dim[flash.dimScreenTimer];
}
uint8_t MemoryFlash::getScreenBrightness(void)
{
    return flash.screenBrightness;
}

MemoryFlash data;