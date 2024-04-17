/*
 * stabilityMonitorTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

// Standard library includes
#include <stdio.h>
#include <stdint.h>

// OS includes
#include "../freertos/FreeRTOS.h"

// Source includes
#include "../globals.h"
#include "../queues.h"
#include "defines.h"

#include "stabilityMonitorTask.h"

void stabilityMonitorTask(void *pvParameters)
{
	uint8_t currentStable = -1; // start with invalid stability to force update
	double prevFreq = -1;
	double instantFreq = -1;

	TickType_t prevTick = xTaskGetTickCount();
	TickType_t currentTick;

	for (;;) { 
		// Check if there's something in the queue
		if (xQueueReceive(newFreqQ, &instantFreq , 0) == pdPASS) {
			// Get Tick
			currentTick = xTaskGetTickCount();


			// check if frequency above threshold
			uint8_t frequency_stable = instantFreq > THRESHOLD_FREQUENCY ? 1 : 0;



			// Get ROC
			float timeDifference = (currentTick - prevTick)/(float)configTICK_RATE_HZ;
			float freqROC = (instantFreq-prevFreq) /timeDifference;

			// handle first loop where prevFreq is unset
			if (prevFreq == -1)
			{
				freqROC = 0;
			}
			//Check ROC in bounds
			uint8_t rateOfChangeStabe = freqROC > THRESHOLD_ROC_FREQUENCY ? 1 : 0;

			uint8_t isStable = frequency_stable && rateOfChangeStabe;

			// if stability has changed set the global variable
			if (isStable != currentStable) {
				printf("\r\nStability is now %i with a freq of %.2f and ROC of %.3f", isStable, instantFreq, freqROC); //
				
				if (xSemaphoreTake(xisStableMutex, portMAX_DELAY) == pdPASS) {
					xisStable = isStable; // Set the global variable
				}
				xSemaphoreGive(xisStableMutex);
			
				// update current state
				currentStable = isStable;
				prevFreq = instantFreq;

			}
			
		}

		vTaskDelay(20);
	}
}
