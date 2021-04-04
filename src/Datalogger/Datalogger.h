#ifndef __DATALOG_H__
#define __DATALOG_H__

#include "Arduino.h"

class Datalogger
{
public:
    void setting(uint8_t loggerType);

private:
    void scheduler(uint8_t loggerType);
    void showBaudrateOption(bool type, bool show);
    void updateSelectedBaudrateToNextion(bool type, uint8_t selected);
    void showObjDatalogPage(uint8_t loggerType, bool show);
    void updateStrPeriod(uint8_t hourPeriod, uint8_t minutePeriod, uint8_t secondPeriod);
};

extern Datalogger logger;
#endif