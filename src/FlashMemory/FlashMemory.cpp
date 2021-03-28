#include "FlashMemory.h"
#include "EEPROM.h"
#include "../DebugSerial/DebugSerial.h"

bool MemoryFlash::begin(uint16_t sizeOfMemory)
{
    //init setup flash memory to use 512 byte storage
    if (EEPROM.begin(sizeOfMemory))
    {
        readAll();
        Serial.begin(115200);
        printDebug(getDebugMode(), "Initialization of flash succesful!");
        Serial.end();
        return true;
    }
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
            flash.fieldChannel[i] = i;
            flash.pointCalibrationStatus[i] = 0;
            flash.gramMaximum[i] = 0;
            for (uint8_t j = 0; j < MAX_POINT_CAL; j++)
                flash.adcCalibrationPoint[i][j] = 0;
            for (uint8_t k = 0; k < MAX_POINT_CAL - 1; k++)
                flash.gramCalibrationPoint[i][k] = 0.0;
        }
        flash.batteryCapacity = 1000;
        flash.periodDataLog[SERIAL1] = 1;
        flash.periodDataLog[LOCAL] = 1;
        flash.periodDataLog[WEB] = 180;
        flash.baudrateSerial[DEBUG] = 6;
        flash.baudrateSerial[LOGGER] = 6;
        flash.dimScreenTimer = 0;
        flash.screenBrightness = 100;
        storeDataToFlash();
    }
    else
        return false;

    return true;
}

void MemoryFlash::readAll()
{

    EEPROM.get(0, flash);
    printDebug(getDebugMode(), "All flash data read!");
}

////update data struct flash to flash memory
void MemoryFlash::storeDataToFlash(void)
{
    EEPROM.put(0, flash);
    if (EEPROM.commit())
        printDebug(getDebugMode(), "Stored succesfully!");
    else
        printDebug(getDebugMode(), "failed to stored!!!");
}

////setting data flash
bool MemoryFlash::setSSID(const char *newValue)
{
    if (strcmp(newValue, "") == 0)
    {
        printDebug(getDebugMode(), String() + "Data Failed to be stored!");
        return false;
    }

    if (strcmp(newValue, flash.ssid)) //if data is different
    {
        strcpy(flash.ssid, newValue);
        storeDataToFlash();
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setPassword(const char *newValue)
{
    if (strcmp(newValue, "") == 0)
    {
        printDebug(getDebugMode(), String() + "Data Failed to be stored!");
        return false;
    }
    if (strcmp(newValue, flash.password)) //if data is different
    {
        strcpy(flash.password, newValue);
        storeDataToFlash();
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setKeyAPI(const char *newValue)
{
    if (strcmp(newValue, "") == 0)
    {
        printDebug(getDebugMode(), String() + "Data Failed to be stored!");
        return false;
    }
    if (strcmp(newValue, flash.ssid)) //if data is different
    {
        strcpy(flash.keyAPI, newValue);
        storeDataToFlash();
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setEncryptType(uint8_t newValue)
{
    if (newValue != flash.encryptType)
    {
        flash.encryptType = newValue;
        storeDataToFlash();
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setTimezone(uint8_t newValue)
{
    if (newValue != flash.timeZone)
    {
        flash.timeZone = newValue;
        storeDataToFlash();
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setMeasurementUnit(uint8_t newValue)
{
    if (newValue != flash.measurementUnit)
    {
        flash.measurementUnit = newValue;
        storeDataToFlash();
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
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
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
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
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
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
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setDatalogStatus(uint8_t loggerType, bool newValue)
{
    uint8_t type = 0;

    if (loggerType > WEB)
        return false;

    if (loggerType == SERIAL1)
        type = SERIAL_LOG_ENABLE;
    else if (loggerType == LOCAL)
        type = LOCAL_LOG_ENABLE;
    else if (loggerType == WEB)
        type = WEB_LOG_ENABLE;

    if (newValue != bitRead(flash.generalStatus, type))
    {
        if (newValue)
            bitSet(flash.generalStatus, type);
        else
            bitClear(flash.generalStatus, type);
        storeDataToFlash();
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}
// bool MemoryFlash::setSerialDatalogStatus(bool newValue)
// {
//     if (newValue != bitRead(flash.generalStatus, SERIAL_LOG_ENABLE))
//     {
//         if (newValue)
//             bitSet(flash.generalStatus, SERIAL_LOG_ENABLE);
//         else
//             bitClear(flash.generalStatus, SERIAL_LOG_ENABLE);
//         storeDataToFlash();
//         printDebug(getDebugMode(), String() + newValue + ": new data set!");
//         return true;
//     }
//     else
//     {
//         printDebug(getDebugMode(), String() + "New data already in flash!");
//     }
//     return false;
// }
// bool MemoryFlash::setWebDatalogStatus(bool newValue)
// {
//     if (newValue != bitRead(flash.generalStatus, WEB_LOG_ENABLE))
//     {
//         if (newValue)
//             bitSet(flash.generalStatus, WEB_LOG_ENABLE);
//         else
//             bitClear(flash.generalStatus, WEB_LOG_ENABLE);
//         storeDataToFlash();
//         printDebug(getDebugMode(), String() + newValue + ": new data set!");
//         return true;
//     }
//     else
//     {
//         printDebug(getDebugMode(), String() + "New data already in flash!");
//     }
//     return false;
// }

bool MemoryFlash::setBatteryCapacity(uint16_t newValue)
{
    if (newValue != flash.batteryCapacity)
    {
        flash.batteryCapacity = newValue;
        storeDataToFlash();
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}

bool MemoryFlash::setFieldChannel(uint8_t channel, uint8_t newValue)
{
    if (newValue != flash.fieldChannel[channel])
    {
        flash.fieldChannel[channel] = newValue;
        storeDataToFlash();
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setPointCalibrationStatus(uint8_t channel, uint8_t point, bool newValue)
{
    if (newValue != bitRead(flash.pointCalibrationStatus[channel], point))
    {
        if (newValue)
            bitSet(flash.pointCalibrationStatus[channel], point);
        else
            bitClear(flash.pointCalibrationStatus[channel], point);
        storeDataToFlash();
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setPeriodDatalog(uint8_t loggerType, uint32_t newValue)
{
    if (loggerType > WEB)
        return false;
    if (newValue != flash.periodDataLog[loggerType])
    {
        flash.periodDataLog[loggerType] = newValue;
        storeDataToFlash();
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}
// bool MemoryFlash::setPeriodWebDatalog(uint32_t newValue)
// {
//     if (newValue != flash.periodWebDataLog)
//     {
//         flash.periodWebDataLog = newValue;
//         storeDataToFlash();
//         printDebug(getDebugMode(), String() + newValue + ": new data set!");
//         return true;
//     }
//     else
//     {
//         printDebug(getDebugMode(), String() + "New data already in flash!");
//     }
//     return false;
// }
// bool MemoryFlash::setPeriodSerialDatalog(uint32_t newValue)
// {
//     if (newValue != flash.periodSerial1DataLog)
//     {
//         flash.periodSerial1DataLog = newValue;
//         storeDataToFlash();
//         printDebug(getDebugMode(), String() + newValue + ": new data set!");
//         return true;
//     }
//     else
//     {
//         printDebug(getDebugMode(), String() + "New data already in flash!");
//     }
//     return false;
// }
bool MemoryFlash::setBaudrateSerial(bool type, uint8_t newValue)
{
    if (newValue != flash.baudrateSerial[type])
    {
        flash.baudrateSerial[type] = newValue;
        storeDataToFlash();
        printDebug(getDebugMode(), String() + baudrate[newValue] + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setAdcCalibrationPoint(uint8_t channel, uint8_t point, uint32_t newValue)
{
    if (newValue != flash.adcCalibrationPoint[channel][point])
    {
        flash.adcCalibrationPoint[channel][point] = newValue;
        storeDataToFlash();
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setGramCalibrationPoint(uint8_t channel, uint8_t point, float newValue)
{
    if (newValue != flash.gramCalibrationPoint[channel][point])
    {
        flash.gramCalibrationPoint[channel][point] = newValue;
        storeDataToFlash();
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}

bool MemoryFlash::setGramMaximum(uint8_t channel, float newValue)
{
    if (newValue != flash.gramMaximum[channel])
    {
        flash.gramMaximum[channel] = newValue;
        storeDataToFlash();
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}

bool MemoryFlash::setOffScheduleDatalog(uint8_t loggerType, uint8_t index, uint16_t newValue)
{
    uint16_t temp1 = newValue & 0x0FFF;
    uint16_t temp2 = flash.offScheduleDatalog[loggerType][index] & 0x0FFF;
    uint16_t temp3 = (flash.offScheduleDatalog[loggerType][index] & 0x1000);

    if (newValue > 0x0FFF)
        return false;
    if (temp1 != temp2)
    {
        temp2 = temp1;
        flash.offScheduleDatalog[loggerType][index] = temp3 | temp2;

        storeDataToFlash();
        printDebug(getDebugMode(), String() + temp2 + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setOnScheduleDatalog(uint8_t loggerType, uint8_t index, uint16_t newValue)
{
    uint16_t temp1 = newValue & 0x0FFF;
    uint16_t temp2 = flash.onScheduleDatalog[loggerType][index] & 0x0FFF;
    uint16_t temp3 = (flash.onScheduleDatalog[loggerType][index] & 0x1000);

    if (newValue > 0x0FFF)
        return false;
    if (temp1 != temp2)
    {
        temp2 = temp1;
        flash.onScheduleDatalog[loggerType][index] = temp3 | temp2;
        storeDataToFlash();
        printDebug(getDebugMode(), String() + temp2 + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}

bool MemoryFlash::setEnableScheduleOff(uint8_t loggerType, uint8_t index, bool newValue)
{
    bool oldValue = false;
    uint16_t temp1 = flash.offScheduleDatalog[loggerType][index] & 0x1000;

    if (temp1 > 0)
        oldValue = true;
    else
        oldValue = false;

    if (newValue != oldValue)
    {
        oldValue = newValue;
        if (oldValue)
            flash.offScheduleDatalog[loggerType][index] |= 0x1000;
        else
            flash.offScheduleDatalog[loggerType][index] &= 0xEFFF;

        storeDataToFlash();
        printDebug(getDebugMode(), String() + oldValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}

bool MemoryFlash::setEnableScheduleOn(uint8_t loggerType, uint8_t index, bool newValue)
{
    bool oldValue = false;
    uint16_t temp1 = flash.onScheduleDatalog[loggerType][index] & 0x1000;

    if (temp1 > 0)
        oldValue = true;
    else
        oldValue = false;

    if (newValue != oldValue)
    {
        oldValue = newValue;
        if (oldValue)
            flash.onScheduleDatalog[loggerType][index] |= 0x1000;
        else
            flash.onScheduleDatalog[loggerType][index] &= 0xEFFF;

        storeDataToFlash();
        printDebug(getDebugMode(), String() + oldValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setDimScreenTimer(uint8_t newValue)
{
    if (newValue != flash.dimScreenTimer)
    {
        flash.dimScreenTimer = newValue;
        storeDataToFlash();
        printDebug(getDebugMode(), String() + dim[newValue] + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
    }
    return false;
}
bool MemoryFlash::setScreenBrightness(uint8_t newValue)
{
    if (newValue != flash.screenBrightness)
    {
        flash.screenBrightness = newValue;
        storeDataToFlash();
        printDebug(getDebugMode(), String() + newValue + ": new data set!");
        return true;
    }
    else
    {
        printDebug(getDebugMode(), String() + "New data already in flash!");
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
    uint8_t type;
    if (loggerType > WEB)
        return false;

    if (loggerType == SERIAL1)
        type = SERIAL_LOG_ENABLE;
    else if (loggerType == LOCAL)
        type = LOCAL_LOG_ENABLE;
    else if (loggerType == WEB)
        type = WEB_LOG_ENABLE;

    return bitRead(flash.generalStatus, type);
}
// bool MemoryFlash::getSerialDatalogStatus(void)
// {
//     return bitRead(flash.generalStatus, SERIAL_LOG_ENABLE);
// }
// bool MemoryFlash::getWebDatalogStatus(void)
// {
//     return bitRead(flash.generalStatus, WEB_LOG_ENABLE);
// }
uint8_t MemoryFlash::getFieldChannel(uint8_t channel)
{
    return flash.fieldChannel[channel];
}
bool MemoryFlash::getPointCalibrationStatus(uint8_t channel, uint8_t point)
{
    return bitRead(flash.pointCalibrationStatus[channel], point);
}
uint16_t MemoryFlash::getBatteryCapacity(void)
{
    return flash.batteryCapacity;
}
uint32_t MemoryFlash::getPeriodDatalog(uint8_t loggerType)
{
    if (loggerType > WEB)
        return 0;
    return flash.periodDataLog[loggerType];
}
// uint32_t MemoryFlash::getPeriodWebDatalog(void)
// {
//     return flash.periodWebDataLog;
// }
// uint32_t MemoryFlash::getPeriodSerialDatalog(void)
// {
//     return flash.periodSerial1DataLog;
// }
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
uint16_t MemoryFlash::getOffScheduleDatalog(uint8_t LoggerType, uint8_t index)
{
    return flash.offScheduleDatalog[LoggerType][index] & 0x0FFF;
}
uint16_t MemoryFlash::getOnScheduleDatalog(uint8_t LoggerType, uint8_t index)
{
    return flash.onScheduleDatalog[LoggerType][index] & 0x0FFF;
}
bool MemoryFlash::getEnableScheduleOn(uint8_t loggerType, uint8_t index)
{
    return ((flash.onScheduleDatalog[loggerType][index] & 0x1000) > 0 ? true : false);
}
bool MemoryFlash::getEnableScheduleOff(uint8_t loggerType, uint8_t index)
{
    return ((flash.offScheduleDatalog[loggerType][index] & 0x1000) > 0 ? true : false);
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