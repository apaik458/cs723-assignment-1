/*
 * shouldReconnectTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#include "shouldReconnectTask.h"
#include "../globals.h"
#include "../queues.h"

#include "../freertos/FreeRTOS.h"
#include "../freertos/task.h"

#include <stdio.h>
#include <stdint.h>


void shouldReconnectTask(void *pvParameters)
{
	uint8_t currentlyStable = 0;
	for (;;) {
		if (xSemaphoreTake(xisStableMutex, portMAX_DELAY) == pdPASS) {
			currentlyStable = xisStable;
			xSemaphoreGive(xisStableMutex);
		}

		if ( currentlyStable == 1) {
			// Stable and should start timer
			vTaskDelay(pdMS_TO_TICKS(500));
			if (xSemaphoreTake(xisStableMutex, portMAX_DELAY) == pdPASS) {
				currentlyStable = xisStable;
				xSemaphoreGive(xisStableMutex);
			}
			if (currentlyStable == 1) {
				xQueueSend(reconnectOrShedQ, RECONNECT, 0);
				printf("R");
			}
		} else {
			// else wait for 5 ms cause not being too greedy
			vTaskDelay(pdMS_TO_TICKS(5));
		}

	}
}
