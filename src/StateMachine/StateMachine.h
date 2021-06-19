#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H

#include "Arduino.h"

enum MachineState
{
    HOME,
    NETWORK,
    SETTINGS,
    UNITS,
    DATALOG
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

private:
    bool initFlash(uint16_t memory);
    bool initTime(void);
    void setupPinIO(void);
    void initSDCard(void);

    void updateButtonToggleStateToNextion(uint8_t channel);
    void updateWeightStringToNextion(void);
    void evenBuzzer(void);
    void updateSignalIndicatorToNextion(uint8_t newValue, bool force);
    void updateBatteryIndicatorToNextion(uint8_t newValue, bool force);
    bool isWeightExceedMaximumValue(uint8_t channel, float actualWeight);
    String getStringUnit(uint8_t unit);
    void updateExceedMaximumFlagToNextion(void);
    void updateSelectedUnitToNextion(uint8_t unit);
    bool batProgressBarShowed = false;
    String prevWeightString[6];
    uint8_t batteryValue = 0;
    uint8_t signalValue = 0;
    bool buzzerState = false;
    bool maxState[6] = {0, 0, 0, 0, 0, 0};
    bool dataLoggingState[3] = {false};
};

#endif