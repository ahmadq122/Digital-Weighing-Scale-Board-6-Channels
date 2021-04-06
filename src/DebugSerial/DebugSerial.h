#ifndef _DEBUG_SERIAL_H_
#define _DEBUG_SERIAL_H_
#include "Arduino.h"

void printDebugln(const char *string);
void printDebugln(String string);
void printDebugln(long number);
void printDebug(const char *string);
void printDebug(String string);
void printDebug(long number);
#endif