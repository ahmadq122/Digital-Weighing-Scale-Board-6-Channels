#ifndef FLASH_MEMORY_h
#define FLASH_MEMORY_h

#include "Arduino.h"

#define MAX_SSID_CHAR 20
#define MAX_PASSWORD_CHAR 15

#define MEMORY_SIZE 512

#define MAX_CHANNEL 6
#define MAX_POINT_CAL 8

// #define DEBUG_ENABLE 0x01
// #define DEBUG_ENABLE 0x01
// #define DEBUG_ENABLE 0x01

// #define PT_0 0x0001
// #define PT_1 0x0002
// #define PT_2 0x0004
// #define PT_3 0x0008
// #define PT_4 0x0010
// #define PT_5 0x0020
// #define PT_6 0x0040
// #define PT_7 0x0080
// #define PT_8 0x0100

// #define CH_1_EN 0x01
// #define CH_2_EN 0x02
// #define CH_3_EN 0x04
// #define CH_4_EN 0x08
// #define CH_5_EN 0x10
// #define CH_6_EN 0x20

enum GeneralStatus
{
    DEBUG_ENABLE,
    NETWORK_ENABLE,
    LOCAL_LOG_ENABLE,
    SERIAL_LOG_ENABLE,
    WEB_LOG_ENABLE
};
enum BaudrateSerial
{
    DEBUG,
    LOGGER
};
enum LoggerType
{
    SERIAL1,
    LOCAL,
    WEB
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
    bool setBatteryCapacity(uint16_t newValue);
    bool setFieldChannel(uint8_t channel, uint8_t newValue);
    bool setPointCalibrationStatus(uint8_t channel, uint8_t point, bool newValue);
    bool setPeriodDatalog(uint8_t loggerType, uint32_t newValue);
    bool setBaudrateSerial(bool type, uint32_t newValue);
    bool setAdcCalibrationPoint(uint8_t channel, uint8_t point, uint32_t newValue);
    bool setGramCalibrationPoint(uint8_t channel, uint8_t point, float newValue);
    bool setGramMaximum(uint8_t channel, float newValue);
    bool setOffScheduleDatalog(uint8_t loggerType, uint8_t index, uint16_t newValue);
    bool setOnScheduleDatalog(uint8_t loggerType, uint8_t index, uint16_t newValue);
    bool setEnableScheduleOn(uint8_t loggerType, uint8_t index, bool newValue);
    bool setEnableScheduleOff(uint8_t loggerType, uint8_t index, bool newValue);

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
    uint8_t getFieldChannel(uint8_t channel);
    bool getPointCalibrationStatus(uint8_t channel, uint8_t point);
    uint16_t getBatteryCapacity(void);
    uint32_t getPeriodDatalog(uint8_t loggerType);
    uint32_t getBaudrateSerial(bool type);
    uint32_t getAdcCalibrationPoint(uint8_t channel, uint8_t point);
    float getGramCalibrationPoint(uint8_t channel, uint8_t point);
    float getGramMaximum(uint8_t channel);
    uint16_t getOffScheduleDatalog(uint8_t loggerType, uint8_t index);
    uint16_t getOnScheduleDatalog(uint8_t loggerType, uint8_t index);
    bool getEnableScheduleOn(uint8_t loggerType, uint8_t index);
    bool getEnableScheduleOff(uint8_t loggerType, uint8_t index);

    // struct dataFlash
    // {
    //     char ssid[MAX_SSID_CHAR];
    //     char password[MAX_PASSWORD_CHAR];
    //     char keyAPI[20];
    //     uint8_t encryptType;
    //     uint8_t timeZone = 0;
    //     uint8_t measurementUnit = 0;
    //     uint8_t channelEnDisStatus = B00111111;
    //     uint8_t generalStatus = B00000001; //debugMode + networkEnable (card, serial and web log)
    //     uint8_t fieldChannel[MAX_CHANNEL];
    //     uint8_t pointCalibrationStatus[MAX_CHANNEL];
    //     uint16_t batteryCapacity = 1000;
    //     uint32_t periodDataLog[3];
    //     // uint32_t periodWebDataLog;
    //     // uint32_t periodSerial1DataLog;
    //     uint32_t baudrateSerial;
    //     uint32_t adcCalibrationPoint[MAX_CHANNEL][MAX_POINT_CAL];
    //     float gramCalibrationPoint[MAX_CHANNEL][MAX_POINT_CAL - 1];
    //     float gramMaximum[MAX_CHANNEL];
    //     uint16_t offScheduleDatalog[3][2];
    //     uint16_t onScheduleDatalog[3][2];
    // } flash;

private:
    struct dataFlash
    {
        char ssid[MAX_SSID_CHAR];
        char password[MAX_PASSWORD_CHAR];
        char keyAPI[20];
        uint8_t encryptType;
        uint8_t timeZone = 0;
        uint8_t measurementUnit = 0;
        uint8_t channelEnDisStatus = B00111111;
        uint8_t generalStatus = B00000001; //debugMode + networkEnable (card, serial and web log)
        uint8_t fieldChannel[MAX_CHANNEL];
        uint8_t pointCalibrationStatus[MAX_CHANNEL];
        uint16_t batteryCapacity = 1000;
        uint32_t periodDataLog[3];
        uint32_t baudrateSerial[2];
        uint32_t adcCalibrationPoint[MAX_CHANNEL][MAX_POINT_CAL];
        float gramCalibrationPoint[MAX_CHANNEL][MAX_POINT_CAL - 1];
        float gramMaximum[MAX_CHANNEL];
        uint16_t offScheduleDatalog[3][2];
        uint16_t onScheduleDatalog[3][2];
    } flash;

    bool resetDefault();
    bool flushFlash();
    void storeDataToFlash(void);
};

extern MemoryFlash data;

#endif
