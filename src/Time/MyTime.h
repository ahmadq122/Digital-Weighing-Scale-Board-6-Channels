#ifndef __MY_TIME__H__
#define __MY_TIME__H__

#include "RTC/RTCDS1307.h"
#define WIT_OFFSET 9 * 30 * 60 //UTC +9
#define WITA_OFFSET 8 * 30 * 60  //UTC +8
#define WIB_OFFSET 7 * 30 * 60 //UTC +7
#define WIB 0
#define WITA 1
#define WIT 2

class MyTime : RTCDS1307
{
public:
    void initTime(bool *initTimeState, bool *syncroneRTC);
    void initOffset(uint8_t timeZone);
    void updateTime(bool wifiConnected, bool *secondTriggered, bool *initTimeState, bool *syncroneRTC);
    bool getMyLocalTime(bool *initTimeState, bool *syncroneRTC);
    bool updateRTC_N_NTPTime(bool wifiConnected, bool *secondTriggered, bool *initTimeState, bool *syncroneRTC);
    void getTimeStr(char *buffer);
    void getDateStr(char *buffer);
    void getTimeAndDateStr(char *buffer);
    const char *ntpServer = "asia.pool.ntp.org";
    long gmtOffset_sec = WIB_OFFSET;
    int daylightOffset_sec = WIB_OFFSET;
private:

};

extern MyTime mtime;

#endif