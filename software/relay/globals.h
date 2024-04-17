#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

extern uint8_t xisStable;
extern SemaphoreHandle_t xisStableMutex;

extern TickType_t xfirstTick;
extern SemaphoreHandle_t xfirstTickMutex;

#endif
