
#if defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny85__) || (__AVR_ATtiny2313__)
#include <TinyWireM.h>
#define Wire TinyWireM
#else
#include <Wire.h>
#endif
#include "Arduino.h"
#include "RTCDS1307.h"
#include "FlashMemory/FlashMemory.h"
#include "DebugSerial/DebugSerial.h"

#define DS1307_CTRL_ID 0x68
#define SEC_ADDRS 0x00
#define MIN_ADDRS 0x01
#define HOUR_ADDRS 0x02
#define DAY_ADDRS 0x03
#define DATE_ADDRS 0x04
#define MONTH_ADDRS 0x05
#define YEAR_ADDRS 0x06
#define CTL_ADDRS 0x07
#define RTC_RAM_ADDRS 0x08 //0x08~0x3F (56 Bytes)
#define UNAVAILABLE false
#define AVAILABLE true

bool RTCDS1307::begin()
{
    bool ret = Wire.begin();
    if (ret)
        readAll();
    return ret;
}

bool RTCDS1307::setTimeFormat(uint8_t format)
{
    uint8_t form = 0;
    rtcData d;

    if (!readTime())
        return false;
    d.second = rtc.second;
    d.minute = rtc.minute;
    d.hour = rtc.hour;
    d.timeFormat = rtc.timeFormat;
    d.ampm = HOUR_24;
    if (d.timeFormat == HOUR_12)
        d.ampm = rtc.ampm;
    if (d.timeFormat == format)
        return true;
    else
    {
        form = (form | format) << 6;
        Wire.beginTransmission(DS1307_CTRL_ID);
#if ARDUINO >= 100
        Wire.write(HOUR_ADDRS);
        Wire.write(form);
#else
        Wire.send(HOUR_ADDRS);
        Wire.send(form);
#endif
        if (Wire.endTransmission() != 0)
        {
            available = UNAVAILABLE;
            return UNAVAILABLE;
        }
    }
    if (format == HOUR_24)
    {
        if (d.ampm == PM)
        {
            if (d.hour < 12)
                d.hour += 12;
        }
        if (!setTime(d.hour, d.minute, d.second))
        {
            available = UNAVAILABLE;
            return UNAVAILABLE;
        }
    }
    else
    {
        if (d.hour > 12)
        {
            d.hour -= 12;
            if (!setTime(d.hour, d.minute, d.second, PM))
            {
                available = UNAVAILABLE;
                return UNAVAILABLE;
            }
        }
        else if (d.hour == 12)
        {
            if (!setTime(d.hour, d.minute, d.second, PM))
            {
                available = UNAVAILABLE;
                return UNAVAILABLE;
            }
        }
        else
        {
            if (!setTime(d.hour, d.minute, d.second, AM))
            {
                available = UNAVAILABLE;
                return UNAVAILABLE;
            }
        }
    }
    available = AVAILABLE;
    return AVAILABLE;
}

///////////////////////WRITING
bool RTCDS1307::setTime(uint8_t hour, uint8_t minute, uint8_t second)
{
    uint8_t temp_hour = 0;
    uint8_t temp_minute = 0;
    uint8_t temp_second = 0;

    hour = constrain(hour, 0, 23);
    if (hour <= 9)
        temp_hour = 0x00;
    else if (hour > 9 && hour <= 19)
        temp_hour = 0x10;
    else
        temp_hour = 0x20;
    hour -= (temp_hour >> 4) * 10;
    hour |= temp_hour;
    //    hour &= 0x5F;//ensure time format to 24 hour

    minute = constrain(minute, 0, 59);
    if (minute <= 9)
        temp_minute = 0x00;
    else if (minute > 9 && minute <= 19)
        temp_minute = 0x10;
    else if (minute > 19 && minute <= 29)
        temp_minute = 0x20;
    else if (minute > 29 && minute <= 39)
        temp_minute = 0x30;
    else if (minute > 39 && minute <= 49)
        temp_minute = 0x40;
    else
        temp_minute = 0x50;
    minute -= (temp_minute >> 4) * 10;
    minute |= temp_minute;

    second = constrain(second, 0, 59);
    if (second <= 9)
        temp_second = 0x00;
    else if (second > 9 && second <= 19)
        temp_second = 0x10;
    else if (second > 19 && second <= 29)
        temp_second = 0x20;
    else if (second > 29 && second <= 39)
        temp_second = 0x30;
    else if (second > 39 && second <= 49)
        temp_second = 0x40;
    else
        temp_second = 0x50;
    second -= (temp_second >> 4) * 10;
    second |= temp_second;

    Wire.beginTransmission(DS1307_CTRL_ID);
#if ARDUINO >= 100
    Wire.write(SEC_ADDRS);
    Wire.write(second);
    Wire.write(minute);
    Wire.write(hour);
#else
    Wire.send(SEC_ADDRS);
    Wire.send(second);
    Wire.send(minute);
    Wire.send(hour);
#endif
    if (Wire.endTransmission() != 0)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }
    available = AVAILABLE;
    return AVAILABLE;
}

bool RTCDS1307::setTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t ampm)
{
    uint8_t temp_hour = 0;
    uint8_t temp_minute = 0;
    uint8_t temp_second = 0;
    uint8_t temp_ampm = 0;

    temp_ampm = ampm == PM ? 0x20 : 0xDF;

    hour = constrain(hour, 0, ampm == PM ? 12 : 11);
    if (hour <= 9)
        temp_hour = 0x00;
    else
        temp_hour = 0x10;
    hour -= (temp_hour >> 4) * 10;
    hour |= temp_hour;
    hour = ampm == PM ? hour | temp_ampm : hour & temp_ampm;
    //    hour |= 0x40;//ensure time format to 12 hour

    minute = constrain(minute, 0, 59);
    if (minute <= 9)
        temp_minute = 0x00;
    else if (minute > 9 && minute <= 19)
        temp_minute = 0x10;
    else if (minute > 19 && minute <= 29)
        temp_minute = 0x20;
    else if (minute > 29 && minute <= 39)
        temp_minute = 0x30;
    else if (minute > 39 && minute <= 49)
        temp_minute = 0x40;
    else
        temp_minute = 0x50;
    minute -= (temp_minute >> 4) * 10;
    minute |= temp_minute;

    second = constrain(second, 0, 59);
    if (second <= 9)
        temp_second = 0x00;
    else if (second > 9 && second <= 19)
        temp_second = 0x10;
    else if (second > 19 && second <= 29)
        temp_second = 0x20;
    else if (second > 29 && second <= 39)
        temp_second = 0x30;
    else if (second > 39 && second <= 49)
        temp_second = 0x40;
    else
        temp_second = 0x50;
    second -= (temp_second >> 4) * 10;
    second |= temp_second;

    Wire.beginTransmission(DS1307_CTRL_ID);
#if ARDUINO >= 100
    Wire.write(SEC_ADDRS);
    Wire.write(second);
    Wire.write(minute);
    Wire.write(hour);
#else
    Wire.send(SEC_ADDRS);
    Wire.send(second);
    Wire.send(minute);
    Wire.send(hour);
#endif
    if (Wire.endTransmission() != 0)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }
    available = AVAILABLE;
    return AVAILABLE;
}

bool RTCDS1307::setDay(uint8_t day)
{
    day = constrain(day, 1, 7);
    Wire.beginTransmission(DS1307_CTRL_ID);
#if ARDUINO >= 100
    Wire.write(DAY_ADDRS);
    Wire.write(day);
#else
    Wire.send(DAY_ADDRS);
    Wire.send(day);
#endif
    if (Wire.endTransmission() != 0)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }
    available = AVAILABLE;
    return AVAILABLE;
}

bool RTCDS1307::setDate(uint8_t date, uint8_t month, unsigned int year)
{
    uint8_t temp_date = 0;
    uint8_t temp_month = 0;
    uint8_t temp_year = 0;
    uint8_t years = 0;

    year = constrain(year, 2015, 2099);
    month = constrain(month, 1, 12);
    if (month == 2)
    {
        if (year % 4 == 0)
            date = constrain(date, 1, 29);
        else
            date = constrain(date, 1, 28);
    }
    else
    {
        if (month % 2 == 0)
            date = constrain(date, 1, 30);
        else
            date = constrain(date, 1, 31);
    }

    if (date <= 9)
        temp_date = 0x00;
    else if (date > 9 && date <= 19)
        temp_date = 0x10;
    else if (date > 19 && date <= 29)
        temp_date = 0x20;
    else
        temp_date = 0x03;

    if (month <= 9)
        temp_month = 0x00;
    else
        temp_month = 0x10;

    years = year - 2000;
    if (years <= 9)
        temp_year = 0x00;
    else if (years > 9 && years <= 19)
        temp_year = 0x10;
    else if (years > 19 && years <= 29)
        temp_year = 0x20;
    else if (years > 29 && years <= 39)
        temp_year = 0x30;
    else if (years > 39 && years <= 49)
        temp_year = 0x40;
    else if (years > 49 && years <= 59)
        temp_year = 0x50;
    else if (years > 59 && years <= 69)
        temp_year = 0x60;
    else if (years > 69 && years <= 79)
        temp_year = 0x70;
    else if (years > 79 && years <= 89)
        temp_year = 0x80;
    else
        temp_year = 0x90;

    date = date - ((temp_date >> 4) * 10);
    date |= temp_date;
    month = month - ((temp_month >> 4) * 10);
    month |= temp_month;
    years = years - ((temp_year >> 4) * 10);
    years |= temp_year;

    Wire.beginTransmission(DS1307_CTRL_ID);
#if ARDUINO >= 100
    Wire.write(DATE_ADDRS);
    Wire.write(date);
    Wire.write(month);
    Wire.write(years);
#else
    Wire.send(DATE_ADDRS);
    Wire.send(date);
    Wire.send(month);
    Wire.send(years);
#endif
    if (Wire.endTransmission() != 0)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }
    available = AVAILABLE;
    return UNAVAILABLE;
}

bool RTCDS1307::setDayNDate(uint8_t day, uint8_t date, uint8_t month, unsigned int year)
{
    uint8_t temp_date = 0;
    uint8_t temp_month = 0;
    uint8_t temp_year = 0;
    uint8_t years = 0;

    day = constrain(day, 1, 7);
    year = constrain(year, 2020, 2099);
    month = constrain(month, 1, 12);
    if (month == 2)
    {
        if (year % 4 == 0)
            date = constrain(date, 1, 29);
        else
            date = constrain(date, 1, 28);
    }
    else
    {
        if (month % 2 == 0)
            date = constrain(date, 1, 30);
        else
            date = constrain(date, 1, 31);
    }

    if (date <= 9)
        temp_date = 0x00;
    else if (date > 9 && date <= 19)
        temp_date = 0x10;
    else if (date > 19 && date <= 29)
        temp_date = 0x20;
    else
        temp_date = 0x03;

    if (month <= 9)
        temp_month = 0x00;
    else
        temp_month = 0x10;

    years = year - 2000;
    if (years <= 9)
        temp_year = 0x00;
    else if (years > 9 && years <= 19)
        temp_year = 0x10;
    else if (years > 19 && years <= 29)
        temp_year = 0x20;
    else if (years > 29 && years <= 39)
        temp_year = 0x30;
    else if (years > 39 && years <= 49)
        temp_year = 0x40;
    else if (years > 49 && years <= 59)
        temp_year = 0x50;
    else if (years > 59 && years <= 69)
        temp_year = 0x60;
    else if (years > 69 && years <= 79)
        temp_year = 0x70;
    else if (years > 79 && years <= 89)
        temp_year = 0x80;
    else
        temp_year = 0x90;

    date = date - ((temp_date >> 4) * 10);
    date |= temp_date;
    month = month - ((temp_month >> 4) * 10);
    month |= temp_month;
    years = years - ((temp_year >> 4) * 10);
    years |= temp_year;
    Wire.beginTransmission(DS1307_CTRL_ID);
#if ARDUINO >= 100
    Wire.write(DAY_ADDRS);
    Wire.write(day);
    Wire.write(date);
    Wire.write(month);
    Wire.write(years);
#else
    Wire.send(DAY_ADDRS);
    Wire.send(day);
    Wire.send(date);
    Wire.send(month);
    Wire.send(years);
#endif
    if (Wire.endTransmission() != 0)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }
    available = AVAILABLE;
    return AVAILABLE;
}

///////////////////////READING
bool RTCDS1307::readTime()
{
    rtcData t;
    uint8_t sec;
    uint8_t hour_temp;
    Wire.beginTransmission(DS1307_CTRL_ID);
#if ARDUINO >= 100
    Wire.write(SEC_ADDRS);
#else
    Wire.send(SEC_ADDRS);
#endif
    if (Wire.endTransmission() != 0)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }

    Wire.requestFrom(DS1307_CTRL_ID, 3, 1);
    if (Wire.available() < 3)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }
#if ARDUINO >= 100
    sec = Wire.read();
    t.second = bcd2dec(sec & 0x7f);
    t.minute = bcd2dec(Wire.read());
    hour_temp = Wire.read();
    t.timeFormat = (hour_temp & 0x40) >> 6;
    if (t.timeFormat == HOUR_12)
        t.ampm = (hour_temp & 0x20) >> 5;
    t.hour = t.timeFormat == HOUR_24 ? bcd2dec(hour_temp & 0x3f) : bcd2dec(hour_temp & 0x1f);
#else
    sec = Wire.receive();
    t.second = bcd2dec(sec & 0x7f);
    t.minute = bcd2dec(Wire.receive());
    hour_temp = Wire.receive();
    t.timeFormat = (hour_temp & 0x40) >> 6;
    if (t.timeFormat == HOUR_12)
        t.ampm = (hour_temp & 0x20) >> 5;
    t.hour = t.timeFormat == HOUR_24 ? bcd2dec(hour_temp & 0x3f) : bcd2dec(hour_temp & 0x1f);

#endif
    if (sec & 0x80)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    } // clock is halted

    rtc = t;
    available = AVAILABLE;
    return AVAILABLE;
}

bool RTCDS1307::readTimeFormat()
{
    rtcData d;
    Wire.beginTransmission(DS1307_CTRL_ID);
#if ARDUINO >= 100
    Wire.write(HOUR_ADDRS);
#else
    Wire.send(HOUR_ADDRS);
#endif
    if (Wire.endTransmission() != 0)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }

    Wire.requestFrom(DS1307_CTRL_ID, 1, 1);
    if (Wire.available() < 1)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }
#if ARDUINO >= 100
    d.timeFormat = (Wire.read() & 0x40) >> 6;
#else
    d.timeFormat = (Wire.receive() & 0x40) >> 6;
#endif
    rtc = d;
    available = AVAILABLE;
    return AVAILABLE;
}

bool RTCDS1307::readDay()
{
    rtcData d;
    Wire.beginTransmission(DS1307_CTRL_ID);
#if ARDUINO >= 100
    Wire.write(DAY_ADDRS);
#else
    Wire.send(DAY_ADDRS);
#endif
    if (Wire.endTransmission() != 0)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }

    Wire.requestFrom(DS1307_CTRL_ID, 1, 1);
    if (Wire.available() < 1)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }
#if ARDUINO >= 100
    d.day = bcd2dec(Wire.read());
#else
    d.day = bcd2dec(Wire.receive());
#endif
    rtc = d;
    available = AVAILABLE;
    return AVAILABLE;
}

bool RTCDS1307::readDate()
{
    rtcData d;
    Wire.beginTransmission(DS1307_CTRL_ID);
#if ARDUINO >= 100
    Wire.write(DATE_ADDRS);
#else
    Wire.write(DATE_ADDRS);
#endif
    if (Wire.endTransmission() != 0)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }

    Wire.requestFrom(DS1307_CTRL_ID, 3, 1);
    if (Wire.available() < 3)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }
#if ARDUINO >= 100
    d.date = bcd2dec(Wire.read());
    d.month = bcd2dec(Wire.read());
    d.year = bcd2dec(Wire.read());

    //    d.date = (d.date & 0x0F) + (((d.date & 0x30) >> 4) * 10);
    //    d.month = (d.month & 0x0F) + (((d.month & 0x10) >> 4) * 10);
    //    d.year = (d.year & 0x0F) + (((d.year & 0xF0) >> 4) * 10);
#else
    d.date = bcd2dec(Wire.receive());
    d.month = bcd2dec(Wire.receive());
    d.year = bcd2dec(Wire.receive());
#endif
    rtc = d;
    available = AVAILABLE;
    return AVAILABLE;
}

bool RTCDS1307::readDayNDate()
{
    rtcData d;
    Wire.beginTransmission(DS1307_CTRL_ID);
#if ARDUINO >= 100
    Wire.write(DAY_ADDRS);
#else
    Wire.send(DAY_ADDRS);
#endif
    if (Wire.endTransmission() != 0)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }

    Wire.requestFrom(DS1307_CTRL_ID, 4, 1);
    if (Wire.available() < 4)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }
#if ARDUINO >= 100
    d.day = bcd2dec(Wire.read());
    d.date = bcd2dec(Wire.read());
    d.month = bcd2dec(Wire.read());
    d.year = bcd2dec(Wire.read());
#else
    d.day = bcd2dec(Wire.receive());
    d.date = bcd2dec(Wire.receive());
    d.month = bcd2dec(Wire.receive());
    d.year = bcd2dec(Wire.receive());
#endif
    rtc = d;
    available = AVAILABLE;
    return AVAILABLE;
}

bool RTCDS1307::readAll()
{
    rtcData d;
    uint8_t sec;
    uint8_t hour_temp;
    Wire.beginTransmission(DS1307_CTRL_ID);
#if ARDUINO >= 100
    Wire.write(SEC_ADDRS);
#else
    Wire.send(SEC_ADDRS);
#endif
    if (Wire.endTransmission() != 0)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }

    Wire.requestFrom(DS1307_CTRL_ID, 7, 1);
    if (Wire.available() < 7)
    {
        available = UNAVAILABLE;
        return UNAVAILABLE;
    }
#if ARDUINO >= 100
    sec = Wire.read();
    d.second = bcd2dec(sec & 0x7f);
    d.minute = bcd2dec(Wire.read());
    hour_temp = Wire.read();
    d.timeFormat = (hour_temp & 0x40) >> 6;
    if (d.timeFormat == HOUR_12)
        d.ampm = (hour_temp & 0x20) >> 5;
    d.hour = d.timeFormat == HOUR_24 ? bcd2dec(hour_temp & 0x3f) : bcd2dec(hour_temp & 0x1f);
    d.day = bcd2dec(Wire.read());
    d.date = bcd2dec(Wire.read());
    d.month = bcd2dec(Wire.read());
    d.year = bcd2dec(Wire.read());
#else
    sec = Wire.receive();
    d.second = bcd2dec(sec & 0x7f);
    d.minute = bcd2dec(Wire.receive());
    hour_temp = Wire.receive();
    d.timeFormat = (hour_temp & 0x40) >> 6;
    if (d.timeFormat == HOUR_12)
        d.ampm = (hour_temp & 0x20) >> 5;
    d.hour = d.timeFormat == HOUR_24 ? bcd2dec(hour_temp & 0x3f) : bcd2dec(hour_temp & 0x1f);
    d.day = bcd2dec(Wire.receive());
    d.date = bcd2dec(Wire.receive());
    d.month = bcd2dec(Wire.receive());
    d.year = bcd2dec(Wire.receive());
#endif
    rtc = d;
    available = AVAILABLE;
    return AVAILABLE;
}

//////////////////GET STRING
void RTCDS1307::getTimeString(char *string)
{
    if (rtc.timeFormat == HOUR_24)
    {
        integerToString(ntpEnabled ? ntp.hour : rtc.hour, string, 2);
        string[2] = ':';
        integerToString(ntpEnabled ? ntp.minute : rtc.minute, &string[3], 2);
        string[5] = ':';
        integerToString(ntpEnabled ? ntp.second : rtc.second, &string[6], 2);
        string[8] = '\0';
    }
    else
    {
        integerToString(ntpEnabled ? ntp.hour : rtc.hour, string, 2);
        string[2] = ':';
        integerToString(ntpEnabled ? ntp.minute : rtc.minute, &string[3], 2);
        string[5] = ':';
        integerToString(ntpEnabled ? ntp.second : rtc.second, &string[6], 2);
        string[8] = ' ';
        if (!ntpEnabled)
        {
            if (rtc.ampm == PM)
                string[9] = 'P';
            else
                string[9] = 'A';
            string[10] = 'M';
        }
        else
        {
            if (ntp.ampm == PM)
                string[9] = 'P';
            else
                string[9] = 'A';
            string[10] = 'M';
        }
        string[11] = '\0';
    }
}

void RTCDS1307::getDayString(byte day, char *string)
{
    switch (day)
    {
    case SUNDAY:
        strcpy(string, "Sun");
        break;
    case MONDAY:
        strcpy(string, "Mon");
        break;
    case TUESDAY:
        strcpy(string, "Tue");
        break;
    case WEDNESDAY:
        strcpy(string, "Wed");
        break;
    case THURSDAY:
        strcpy(string, "Thu");
        break;
    case FRIDAY:
        strcpy(string, "Fri");
        break;
    case SATURDAY:
        strcpy(string, "Sat");
        break;
    default:
        break;
    }
}

void RTCDS1307::getDateString(char *string)
{
    integerToString(ntpEnabled ? ntp.date : rtc.date, string, 2);
    string[2] = '/';
    integerToString(ntpEnabled ? ntp.month : rtc.month, &string[3], 2);
    string[5] = '/';
    integerToString((ntpEnabled ? ntp.year : rtc.year), &string[6], 2);
    string[8] = '\0';
}

//void RTCDS1307::getDayNDateString(char* string){
//    getDayString(string);

//    string[3]=' ';
//    integerToString(ntpEnabled ? ntp.date : rtc.date, &string[4], 2);
//    string[6] = '-';
//    integerToString(ntpEnabled ? ntp.month : rtc.month, &string[7], 2);
//    string[9] = '-';
//    integerToString((ntpEnabled ? ntp.year : rtc.year)+2000, &string[10], 4);
//    string[14] = '\0';
//}

bool RTCDS1307::syncWithNTPTime()
{
    bool a;
    if (rtc.timeFormat == HOUR_12)
    {
        a = setTime(ntp.hour, ntp.minute, ntp.second, ntp.ampm);
    }
    else
    {
        printDebug(String() + ntp.hour + ":" + ntp.minute + ":" + ntp.second);
        a = setTime(ntp.hour, ntp.minute, ntp.second);
    }
    return a;
}

bool RTCDS1307::syncWithNTPDate()
{
    char dayStr[4];
    getDayString(ntp.day, dayStr);
    printDebug(String() + dayStr + " " + ntp.date + "-" + ntp.month + "-" + ntp.year);
    return setDayNDate(ntp.day, ntp.date, ntp.month, 2000 + ntp.year);
}

bool RTCDS1307::syncWithNTPTimeNDate()
{
    char dayStr[4];
    bool a;

    printDebug("Syncronizing RTC and NTP...");
    printDebug("NTP Time:");
    a = syncWithNTPTime();
    a = syncWithNTPDate();
    delay(500);

    readAll();

    getDayString(rtc.day, dayStr);
    printDebug("RTC Time:");
    printDebug(String() + rtc.hour + ":" + rtc.minute + ":" + rtc.second);
    printDebug(String() + dayStr + " " + rtc.date + "-" + rtc.month + "-" + rtc.year);
    return a;
}

// PRIVATE FUNCTIONS
// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t RTCDS1307::dec2bcd(uint8_t num)
{
    return ((num / 10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t RTCDS1307::bcd2dec(uint8_t num)
{
    return ((num / 16 * 10) + (num % 16));
}

void integerToString(uint32_t number, char *buffer, uint8_t len)
{
    String tempStr = String() + number;
    String numbStr;
    char charFirstZero[10];
    uint8_t a = 0, b = 0;

    if (tempStr.length() < len)
    {
        a = len - tempStr.length();
        while (a)
        {
            charFirstZero[b] = '0';
            a--;
            b++;
        }
        charFirstZero[b] = '\0';
        numbStr = String() + charFirstZero;
        numbStr += tempStr;
    }
    else
    {
        numbStr = tempStr;
    }
    strcpy(buffer, numbStr.c_str());
}

RTCDS1307 RTC;