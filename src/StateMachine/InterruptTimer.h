#ifndef _INTERRUPT_TIMER_H
#define _INTERRUPT_TIMER_H

#ifndef ESP32
#error This code is designed to run on ESP32 platform, not Arduino nor ESP8266! Please check your Tools->Board setting.
#endif
//These define's must be placed at the beginning before #include "ESP32TimerInterrupt.h"
// Don't define TIMER_INTERRUPT_DEBUG > 2. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG 1

#define TIMER_INTERVAL_TIMER 1000


#endif