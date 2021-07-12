/*
 * RTCDS1307.h - library for DS1307 RTC
 * This library is intended to be uses with Arduino Time library functions
 */

#ifndef _RTCDS1307_H_
#define _RTCDS1307_H_

#define SUNDAY (0x01)
#define MONDAY (0x02)
#define TUESDAY (0x03)
#define WEDNESDAY (0x04)
#define THURSDAY (0x05)
#define FRIDAY (0x06)
#define SATURDAY (0x07)
#define HOUR_24 (0x00)
#define HOUR_12 (0x01)
#define AM (0x00)
#define PM (0x01)


// library interface description
class RTCDS1307
{
    // user-accessible "public" interface
public:
    struct rtcData
    {
        uint8_t day;
        uint8_t date;
        uint8_t month;
        uint8_t year;
        uint8_t second;
        uint8_t minute;
        uint8_t hour;
        uint8_t timeFormat;
        uint8_t ampm;
    } rtc;

    rtcData ntp;

    bool ntpEnabled;
    bool available;

    bool begin(void);
    bool setTime(uint8_t hour, uint8_t minute, uint8_t second);
    bool setTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t ampm);
    bool setTimeFormat(uint8_t format);
    bool setDay(uint8_t day);
    bool setDate(uint8_t date, uint8_t month, unsigned int year);
    bool setDayNDate(uint8_t day, uint8_t date, uint8_t month, unsigned int year);

    bool readTime();
    bool readTimeFormat();
    bool readDate();
    bool readDayNDate();
    bool readAll();

    void getDayString(byte day, char *string);
    void getDateString(char *string);
    //    void getDayNDateString(char* string);
    void getTimeString(char *string);

    bool syncWithNTPTime();
    bool syncWithNTPDate();
    bool syncWithNTPTimeNDate();
    // void calculateSuperPIN(char *buffer);

private:
    bool readDay();
    uint8_t dec2bcd(uint8_t num);
    uint8_t bcd2dec(uint8_t num);
};

// #ifdef RTC
// #undef RTC //workaround for Arduino Due, which defines "RTC"...
// #endif

// extern RTCDS1307 RTC;

#endif
