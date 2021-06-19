#ifndef FLASH_MEMORY_h
#define FLASH_MEMORY_h

#include "Arduino.h"

#define MAX_SSID_CHAR 16
#define MAX_PASSWORD_CHAR 15
#define MAX_APIKEY_CHAR 18

#define MEMORY_SIZE 512

#define MAX_CHANNEL 6
#define MAX_POINT_CAL 8

enum GeneralStatus
{
    DEBUG_ENABLE,
    NETWORK_ENABLE,
    LOCAL_LOG_ENABLE,
    SERIAL_LOG_ENABLE,
    WEB_LOG_ENABLE,
    SPEED_RATE,
    BUZZER_MUTE
};
enum BaudrateSerial
{
    debugging,
    logging
};
enum LoggerType
{
    serial,
    local,
    remote
};
enum Schedule
{
    _off_,
    _on_
};

enum SpeedRate
{
    SPS_10,
    SPS_80
};

#define DATA_V_BEEN_WRITTEN_FLAG 0xAA

// library interface description
class MemoryFlash
{
    // user-accessible "public" interface
public:
    bool begin(uint16_t sizeOfMemory);
    void readAll();

    //setting data flash
    bool setSSID(const char *newValue);
    bool setPassword(const char *newValue);
    bool setKeyAPI(const char *newValue);
    bool setEncryptType(uint8_t newValue);
    bool setTimezone(uint8_t newValue);
    bool setMeasurementUnit(uint8_t newValue);
    bool setChannelEnDisStatus(uint8_t channel, bool newValue);
    bool setDebugMode(bool newValue);
    bool setNetworkEnable(bool newValue);
    bool setDatalogStatus(uint8_t loggerType, bool newValue);
    // bool setBatteryCapacity(uint16_t newValue);
    // bool setFieldChannel(uint8_t channel, uint8_t newValue);
    bool setPointCalibrationStatus(uint8_t channel, uint8_t point, bool newValue);
    bool setPeriodDatalog(uint8_t loggerType, uint32_t newValue);
    bool setBaudrateSerial(bool type, uint8_t newValue);
    bool setAdcCalibrationPoint(uint8_t channel, uint8_t point, uint32_t newValue);
    bool setGramCalibrationPoint(uint8_t channel, uint8_t point, float newValue);
    bool setGramMaximum(uint8_t channel, float newValue);
    bool setTimeSchedulerDatalog(bool scheduleType, uint8_t loggerType, uint8_t index, uint16_t newValue);
    bool setDateSchedulerDatalog(bool scheduleType, uint8_t loggerType, uint8_t newDate, uint8_t newMonth, uint8_t newYear);
    bool setEnableTimeScheduler(bool scheduleType, uint8_t loggerType, uint8_t index, bool newValue);
    bool setEnableDateScheduler(bool scheduleType, uint8_t loggerType, bool newValue);
    bool setDimScreenTimer(uint8_t newValue);
    bool setScreenBrightness(uint8_t newValue);
    bool setSpeedRate(bool newValue);
    bool setBuzzerMute(bool newValue);
    bool setMaximumBattery(float newValue);
    bool setMinimumBattery(float newValue);

    //reading data flash
    const char *getSSID(void);
    const char *getPassword(void);
    const char *getKeyAPI(void);
    uint8_t getEncryptType(void);
    uint8_t getTimezone(void);
    uint8_t getMeasurementUnit(void);
    bool getChannelEnDisStatus(uint8_t channel);
    bool getDebugMode(void);
    bool getNetworkEnable(void);
    bool getDatalogStatus(uint8_t loggerType);
    // uint8_t getFieldChannel(uint8_t channel);
    bool getPointCalibrationStatus(uint8_t channel, uint8_t point);
    uint8_t getPointCalibrationStatus(uint8_t channel);
    // uint16_t getBatteryCapacity(void);
    uint32_t getPeriodDatalog(uint8_t loggerType);
    uint32_t getBaudrateSerial(bool type);
    uint8_t getBaudrateSerialIndex(bool type);
    uint32_t getAdcCalibrationPoint(uint8_t channel, uint8_t point);
    float getGramCalibrationPoint(uint8_t channel, uint8_t point);
    float getGramMaximum(uint8_t channel);
    void getTimeSchedulerDatalog(bool scheduleType, uint8_t loggerType, uint8_t index, uint8_t *hour, uint8_t *minute);
    void getDateSchedulerDatalog(bool scheduleType, uint8_t loggerType, uint8_t *date, uint8_t *month, uint8_t *year);
    bool getEnableTimeScheduler(bool scheduleType, uint8_t loggerType, uint8_t index);
    bool getEnableDateScheduler(bool scheduleType, uint8_t loggerType);
    uint16_t getDimScreenTimer(void);
    uint8_t getScreenBrightness(void);
    bool getSpeedRate(void);
    bool getBuzzerMute(void);
    float getMaximumBattery(void);
    float getMinimumBattery(void);
    bool resetDefault(void);
    bool resetCalibrationData(uint8_t channel);

    // struct dataFlash
    // {
    //     char ssid[MAX_SSID_CHAR];
    //     char password[MAX_PASSWORD_CHAR];
    //     char keyAPI[MAX_APIKEY_CHAR];
    //     uint8_t encryptType;
    //     uint8_t timeZone = 0;
    //     uint8_t measurementUnit = 0;
    //     uint8_t channelEnDisStatus = B00111111;
    //     uint8_t generalStatus = B00000001; //na, buzzer, spped rate, enable web log, enable serial log, enable card log, networkEnable, debugMode
    //     // uint8_t fieldChannel[MAX_CHANNEL];
    //     uint8_t pointCalibrationStatus[MAX_CHANNEL];
    //     // uint16_t batteryCapacity = 1000;
    //     uint32_t periodDataLog[3];
    //     uint8_t baudrateSerial[2];
    //     uint32_t adcCalibrationPoint[MAX_CHANNEL][MAX_POINT_CAL];
    //     float gramCalibrationPoint[MAX_CHANNEL][MAX_POINT_CAL - 1];
    //     float gramMaximum[MAX_CHANNEL];
    //     uint16_t timeSchedulerDataLog[2][3][2];
    //     uint16_t dateSchedulerDataLog[2][3];
    //     uint8_t enableDateScheduler = 0;
    //     uint8_t dimScreenTimer = 0;
    //     uint8_t screenBrightness = 100;
    //     uint16_t batteryMaximum = 0x0800 | 0x0004;
    //     uint16_t batteryMinimum = 0x0700 | 0x0000;
    // } flash;

private:
    struct dataFlash
    {
        char ssid[MAX_SSID_CHAR];
        char password[MAX_PASSWORD_CHAR];
        char keyAPI[MAX_APIKEY_CHAR];
        uint8_t encryptType;
        uint8_t timeZone = 0;
        uint8_t measurementUnit = 0;
        uint8_t channelEnDisStatus = B00111111;
        uint8_t generalStatus = B00000001; //na, buzzer, spped rate, enable web log, enable serial log, enable card log, networkEnable, debugMode
        // uint8_t fieldChannel[MAX_CHANNEL];
        uint8_t pointCalibrationStatus[MAX_CHANNEL];
        // uint16_t batteryCapacity = 1000;
        uint32_t periodDataLog[3];
        uint8_t baudrateSerial[2];
        uint32_t adcCalibrationPoint[MAX_CHANNEL][MAX_POINT_CAL];
        float gramCalibrationPoint[MAX_CHANNEL][MAX_POINT_CAL - 1];
        float gramMaximum[MAX_CHANNEL];
        uint16_t timeSchedulerDataLog[2][3][2];
        uint16_t dateSchedulerDataLog[2][3];
        uint8_t enableDateScheduler = 0;
        uint8_t dimScreenTimer = 0;
        uint8_t screenBrightness = 100;
        uint16_t batteryMaximum = 0x0800;
        uint16_t batteryMinimum = 0x0700;
    } flash;

    uint32_t baudrate[10] = {
        4800,
        9600,
        19200,
        38400,
        57600,
        74880,
        115200,
        230400,
        250000,
        500000};
    uint16_t dim[6] = {0, 30, 60, 120, 180, 300};

    bool flushFlash();
    bool storeDataToFlash(void);
};

extern MemoryFlash data;

#endif
