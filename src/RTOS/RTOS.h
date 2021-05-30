#ifndef __RTOS__H__
#define __RTOS__H__

#include "Arduino.h"

class RealTimeOS
{
public:
    uint64_t milliSeconds = 0;
    uint32_t counterUpSeconds = 0;
    uint32_t counterDownSeconds = 0;
    uint32_t counterDownSecondsLog[3] = {0};
    uint32_t interruptSeconds = 0;
    uint16_t dimmCounterDownSecond = 30;
    uint8_t currentBrightness = 100;
    bool remoteLogTriggered[6] = {false};
    bool secondBlink = false;
    bool secondTriggered[10] = {false};
    bool wifiConnectionTriggered = {false};
    bool wifiConnected = false;
    bool initTimeState = true;
    bool syncroneRTC = false; //syncronize RTC data with ntp at the first time
    uint8_t startProgressBar = 0;
    void setup(void);
    void updateStartProgressBar(uint8_t add);

private:
};

extern RealTimeOS rtos;
#endif