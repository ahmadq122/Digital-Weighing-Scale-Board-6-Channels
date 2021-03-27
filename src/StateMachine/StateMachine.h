#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H

#include "Arduino.h"

#define AI_Battery 35
#define ADCsample 10

#define rtcd RTC.data

enum MachineState
{
    HOME,
    NETWORK,
    SETTINGS,
    UNITS,
    DATALOG
};
enum Channel
{
    Channel1,
    Channel2,
    Channel3,
    Channel4,
    Channel5,
    Channel6
};

class StateMachine
{
public:
    void setup(void);
    uint8_t homeScreen(void);
    uint8_t networkSettings(void);
    uint8_t settings(void);
    uint8_t measurementUnits(void);
    uint8_t datalogSettings(void);
    float tare[6] = {0};

private:
    float currentWeight[6];
    bool initFlash(uint16_t memory);
    bool initTime(void);
    bool setupInterruptTimer(void);
    void setupPinIO(void);
    void initSDCard(void);

    void updateButtonToggleStateToNextion(uint8_t channel);
    void updateWeightStringToNextion(void);
    void evenBuzzer(void);
    void updateSignalIndicatorToNextion(uint8_t newValue);
    void updateBatteryIndicatorToNextion(uint8_t newValue);
    uint8_t getBatteryPercent(void);
    bool isWeightExceedMaximumValue(uint8_t channel, float actualWeight);
    String getStringUnit(uint8_t unit);
    void updateExceedMaximumFlagToNextion(void);
    void updateSelectedUnitToNextion(uint8_t unit);

    uint16_t adcBatteryContainer[ADCsample];
    String prevWeightString[6];
    uint8_t batteryValue = 0;
    uint8_t signalValue = 0;
    bool maxState[6] = {0, 0, 0, 0, 0, 0};
};

#endif