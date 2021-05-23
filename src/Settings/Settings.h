#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "Arduino.h"

class Settings
{
public:
    void mainMenu(void);    

private:
    void timeZone(void);
    void brightness(void);
    void maximumWeight(void);
    void timeAndDate(void);
    void batteryCapacity(void);
    void debugMenu(void);
    void calibrationSensor(void);
    void setPoint(void);
    void zeroCalibration(void);
    void pointCalibration(void);

    void updateSelectedTimezoneToNextion(void);
    void updateMaximumValueToNextion(void);
    void showBaudrateOption(bool type, bool show);
    void updateSelectedBaudrateToNextion(bool type, uint8_t selected);
    void updateSetpointToNextion(uint8_t channel);
    void updateAllAdcValue(void);
    void updatePointCalibParameter(uint8_t channel, uint8_t point);
};

extern Settings setting;

#endif