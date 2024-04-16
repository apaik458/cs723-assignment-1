/*
 * stabilityMonitorTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */
#include <stdio.h>
#include <stdint.h>

#include "../freertos/FreeRTOS.h"
#include "../freertos/task.h"
#include "../queues.h"
#include "../globals.h"

#include "stabilityMonitorTask.h"
#include "defines.h"

uint8_t currentStable = 2; // start with invalid stability to force update

void stabilityMonitorTask(void *pvParameters)
{
	double instantFreq;

	for (;;) { 
		// Check if there's something in the queue
		if (xQueueReceive(newFreqQ, &instantFreq , 0) == pdPASS) {
			// check if frequency is stable
			uint8_t isStable = instantFreq > THRESHOLD_FREQUENCY ? 1 : 0;

			// if stability has changed set the global variable
			if (isStable != currentStable) {
				if (xSemaphoreTake(xisStableMutex, portMAX_DELAY) == pdPASS) {

					printf("Stability is now %i with a freq of %.3f\r\n", isStable, instantFreq);
					xSemaphoreGive(xisStableMutex);
				}
				
				// update current state
				currentStable = isStable;
			}
		}
	}
}
