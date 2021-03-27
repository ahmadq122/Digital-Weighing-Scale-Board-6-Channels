#include "Arduino.h"
#include "time.h"
#include "MyTime.h"

void MyTime::initTime(bool *initTimeState, bool *syncroneRTC)
{
    //init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    getMyLocalTime(initTimeState, syncroneRTC);
}
void MyTime::initOffset(uint8_t timeZone)
{
    gmtOffset_sec = (timeZone == WIB ? WIB_OFFSET : (timeZone == WITA ? WITA_OFFSET : (timeZone == WIT ? WIT_OFFSET : WIB_OFFSET)));
    daylightOffset_sec = gmtOffset_sec;
}
void MyTime::updateTime(bool wifiConnected, bool *secondTriggered, bool *initTimeState, bool *syncroneRTC)
{
    if (wifiConnected && *initTimeState)
        initTime(initTimeState, syncroneRTC);
    if (!*initTimeState && *secondTriggered)
    {
        getMyLocalTime(initTimeState, syncroneRTC);
        *secondTriggered = false;
    }
}
bool MyTime::getMyLocalTime(bool *initTimeState, bool *syncroneRTC)
{
    struct tm timeinfo;
    char temp[5];
    String sec, min, hour, day, date, month, year, ampm;
    if (!getLocalTime(&timeinfo))
    {
        //    Serial.println("Failed to obtain time");
        ntpEnabled = false;
        return false;
    }

    strftime(temp, sizeof(temp), "%w", &timeinfo);
    day = temp;
    ntp.day = day.toInt() + 1;
    strftime(temp, sizeof(temp), "%d", &timeinfo);
    date = temp;
    ntp.date = date.toInt();
    strftime(temp, sizeof(temp), "%m", &timeinfo);
    month = temp;
    ntp.month = month.toInt();
    strftime(temp, sizeof(temp), "%y", &timeinfo);
    year = temp;
    ntp.year = year.toInt();
    strftime(temp, sizeof(temp), "%S", &timeinfo);
    sec = temp;
    ntp.second = sec.toInt();
    strftime(temp, sizeof(temp), "%M", &timeinfo);
    min = temp;
    ntp.minute = min.toInt();
    strftime(temp, sizeof(temp), rtc.timeFormat ? "%I" : "%H", &timeinfo);
    hour = temp;
    ntp.hour = hour.toInt();
    strftime(temp, sizeof(temp), "%p", &timeinfo);
    ampm = temp;
    ntp.ampm = (ampm == "PM" ? PM : AM);

    // Serial.print(ntp.day);
    // Serial.print(" ");
    // Serial.print(ntp.date);
    // Serial.print("-");
    // Serial.print(ntp.month);
    // Serial.print("-");
    // Serial.print(ntp.year);
    // Serial.print(" ");
    // Serial.print(ntp.hour);
    // Serial.print(":");
    // Serial.print(ntp.minute);
    // Serial.print(":");
    // Serial.print(ntp.second);
    // Serial.print(":");
    // Serial.println(ntp.ampm == AM ? "AM" : "PM");

    *initTimeState = false;
    ntpEnabled = true;
    if (!*syncroneRTC)
    {
        if (syncWithNTPTimeNDate())
            *syncroneRTC = true;
    }
    return true;
}

bool MyTime::updateRTC_N_NTPTime(bool wifiConnected, bool *secondTriggered, bool *initTimeState, bool *syncroneRTC)
{
    if (*secondTriggered)
    { //keep secondTriggered to be used only in this scope
        // Serial.println("Debug!");
        // Serial.println(String() + "wifiConn: " + wifiConnected);
        // Serial.println(String() + "initTime: " + *initTimeState);
        // Serial.println(String() + "syncRTC : " + *syncroneRTC);
        updateTime(wifiConnected, secondTriggered, initTimeState, syncroneRTC);
        readAll();
        *secondTriggered = false;
        return true;
    }
    return false;
}

void MyTime::getTimeStr(char *buffer)
{
    getTimeString(buffer);
}

void MyTime::getDateStr(char *buffer)
{
    getDateString(buffer);
}

void MyTime::getTimeAndDateStr(char *buffer)
{
    char buff[10];
    getTimeString(buff);
    strcpy(buffer, buff);
    getDateString(buff);
    strcat(buffer, " ");
    strcat(buffer, buff);
}

MyTime mtime;