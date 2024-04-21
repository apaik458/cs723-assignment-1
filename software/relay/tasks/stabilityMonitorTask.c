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

void stabilityMonitorTask(void *pvParameters) {
	uint8_t currentStable = -1; // start with invalid stability to force update
	double prevFreq = -1;
	double instantFreq = -1;
	double freqROC;
	double prevFreqROC = 0;

	TickType_t currentTick;

	for (;;) {
		// Check if there's something in the queue
		if (xQueueReceive(newFreqQ, &instantFreq , 0) == pdPASS) {
			// Get Tick
			currentTick = xTaskGetTickCount();

			// check if frequency above threshold
			uint8_t frequency_stable =
					instantFreq > THRESHOLD_FREQUENCY ? 1 : 0;

			// Get ROC
			freqROC = (instantFreq - prevFreq) * 2.0 * instantFreq * prevFreq / (instantFreq + prevFreq);

			//Debounce ROC
			if (prevFreqROC < 0 && freqROC < 0){
				freqROC = prevFreqROC;
			}

			// handle first loop where prevFreq is unset
			if (prevFreq == -1) {
				freqROC = 0;
			}
			// Set previous frequency info
			prevFreqROC = freqROC;
			prevFreq = instantFreq;

			if (xQueueSend(freqMeasureQ, &instantFreq, 0) != pdPASS) {
				printf("ERROR: freqMeasureQ Failed to Send\n");
			}
			//Check ROC in bounds
			uint8_t rateOfChangeStabe =
					freqROC > THRESHOLD_ROC_FREQUENCY ? 1 : 0;
			uint8_t isStable = frequency_stable && rateOfChangeStabe;

			// if stability has changed set the global variable
			if (isStable != currentStable) {
				printf(
						"Stability is now %i with a freq of %.2f and ROC of %.3f\n",
						isStable, instantFreq, freqROC); //

				if (xSemaphoreTake(xisStableMutex, portMAX_DELAY) == pdPASS) {
					xisStable = isStable; // Set the global variable
					xSemaphoreGive(xisStableMutex);
				}

				// Check if switching to be unstable
				if (!isStable) {
					// Save tick to global variable (used for resposne time calculations)
					if (xSemaphoreTake(xfirstTickMutex, portMAX_DELAY) == pdPASS) {
						xfirstTick = currentTick; // Set the global variable
						xSemaphoreGive(xfirstTickMutex);
					}
				}

				// update current state
				currentStable = isStable;

			}

		}

		vTaskDelay(1);
	}
}
