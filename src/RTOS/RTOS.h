#ifndef __RTOS__H__
#define __RTOS__H__

#include "Arduino.h"

class RealTimeOS
{
public:
    uint32_t counterUpSeconds = 0;
    uint32_t counterDownSeconds = 0;
    uint32_t interruptSeconds = 0;
    uint16_t dimmCounterDownSecond = 0;
    uint8_t currentBrightness = 0;
    bool secondBlink = false;
    bool secondTriggered[10] = {false};
    bool wifiConnectionTriggered = {false};
    bool wifiConnected = false;
    bool initTimeState = true;
    bool syncroneRTC = false; //syncronize RTC data with ntp at the first time

    void setup(void);

private:
};

extern RealTimeOS rtos;
#endif