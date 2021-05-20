#include "Arduino.h"
#include "time.h"
#include "MyTime.h"
#include "RTOS/RTOS.h"

void MyTime::initTime(void)
{
    //init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    getMyLocalTime();
}
void MyTime::initOffset(uint8_t timeZone)
{
    gmtOffset_sec = (timeZone == WIB ? WIB_OFFSET : (timeZone == WITA ? WITA_OFFSET : (timeZone == WIT ? WIT_OFFSET : WIB_OFFSET)));
    daylightOffset_sec = gmtOffset_sec;
}
void MyTime::updateTime(void)
{
    if (rtos.wifiConnected && rtos.initTimeState)
        initTime();
    if (!rtos.initTimeState && rtos.secondTriggered[3])
    {
        getMyLocalTime();
        rtos.secondTriggered[3] = false;
    }
}
bool MyTime::getMyLocalTime(void)
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

    rtos.initTimeState = false;
    ntpEnabled = true;
    if (!rtos.syncroneRTC)
    {
        if (syncWithNTPTimeNDate())
            rtos.syncroneRTC = true;
    }
    return true;
}

bool MyTime::updateRTC_N_NTPTime(void)
{
    if (rtos.secondTriggered[4])
    { //keep secondTriggered to be used only in this scope
        // Serial.println("Debug!");
        // Serial.println(String() + "wifiConn: " + rtos.wifiConnected);
        // Serial.println(String() + "initTime: " + rtos.initTimeState);
        // Serial.println(String() + "syncRTC : " + rtos.syncroneRTC);
        updateTime();
        readAll();
        rtos.secondTriggered[4] = false;
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

void MyTime::getActualTimeInMinute(uint16_t *timeMinute)
{
    if (ntpEnabled)
    {
        *timeMinute = (ntp.hour * 60) + ntp.minute;
    }
    else
    {
        *timeMinute = (rtc.hour * 60) + rtc.minute;
    }
}

void MyTime::getRtcTime(int8_t *hour, int8_t *min, int8_t *sec)
{
    readTime();
    *hour = static_cast<int8_t>(rtc.hour);
    *min = static_cast<int8_t>(rtc.minute);
    *sec = static_cast<int8_t>(rtc.second);
}

void MyTime::getRtcDate(int8_t *date, int8_t *month, int8_t *year)
{
    readDate();
    *date = static_cast<int8_t>(rtc.date);
    *month = static_cast<int8_t>(rtc.month);
    *year = static_cast<int8_t>(rtc.year);
}

MyTime mtime;