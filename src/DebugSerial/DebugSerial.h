#ifndef _DEBUG_SERIAL_H_
#define _DEBUG_SERIAL_H_
#include "Arduino.h"

void printDebug(bool enable, const char *string);
void printDebug(bool enable, String string);
void printDebug(bool enable, long number);
#endif