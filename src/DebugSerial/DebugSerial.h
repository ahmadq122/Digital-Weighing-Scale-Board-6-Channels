#ifndef _DEBUG_SERIAL_H_
#define _DEBUG_SERIAL_H_
#include "Arduino.h"

void printDebug(const char *string);
void printDebug(String string);
void printDebug(long number);
#endif