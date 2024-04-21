#ifndef GLOBALS_H_
#define GLOBALS_H_

// os includes
#include "../freertos/FreeRTOS.h"

unsigned int isMaintenanceState;
unsigned int xisStable;
TickType_t xfirstTick;

void initGlobals();

#endif
