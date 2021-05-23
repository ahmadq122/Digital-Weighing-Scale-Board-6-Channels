#ifndef __DATALOG_H__
#define __DATALOG_H__

#include "Arduino.h"

class Datalogger
{
public:
    void setting(uint8_t loggerType);
    bool checkSchedule(bool scheduleType, uint8_t loggerType);
    void logData(uint8_t loggerType);
    void remoteLogging(uint8_t channel);

private:
    void scheduler(uint8_t loggerType);
    void timeScheduler(uint8_t loggerType);
    void dateScheduler(uint8_t loggerType);
    void showBaudrateOption(bool type, bool show);
    void updateSelectedBaudrateToNextion(bool type, uint8_t selected);
    void showObjDatalogPage(uint8_t loggerType, bool show);
    void updateStrPeriod(uint8_t hourPeriod, uint8_t minutePeriod, uint8_t secondPeriod);
    void updateSchedulerSettingState(uint8_t settingState);
    void updateStrOfSetSchedule(uint8_t hourSchedule, uint8_t minuteSchedule);
    void updateTimeScheduler(uint8_t loggerType);
    void updateDateScheduler(uint8_t loggerType);
    void updateSelectedTimeScheduler(uint8_t loggerType, uint8_t select);
    void updateSelectedDateScheduler(uint8_t loggerType, uint8_t select);

    String thingSpeakServer = "https://api.thingspeak.com/update?api_key=";
    uint8_t remoteUpdateForChannel = 0;
};

extern Datalogger logger;
#endif