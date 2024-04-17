#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

uint8_t isMaintenanceState;
uint8_t xisStable;

void initGlobals();

#endif
