#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "Arduino.h"

class Settings
{
public:
    void settingsMenu(void);
    // uint8_t getIndexSelectedBaudrate(bool type);
    void showBaudrateOption(bool type, bool show);
    void updateSelectedBaudrateToNextion(bool type, uint8_t selected);

private:
    void timeZone(void);
    void brightness(void);
    void maximumWeight(void);
    void batteryCapacity(void);
    void debugMenu(void);
    void calibrationSensor(void);
    void setPoint(void);
    void zeroCalibration(void);
    void pointCalibration(void);

    void updateSelectedTimezoneToNextion(void);
    void updateMaximumValueToNextion(void);
};

extern Settings setting;

#endif