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
			// TODO: switch from delay to proper timer, this should be fine for the demo though
			vTaskDelay(pdMS_TO_TICKS(500));
			if (xSemaphoreTake(xisStableMutex, portMAX_DELAY) == pdPASS) {
				currentlyStable = xisStable;
				xSemaphoreGive(xisStableMutex);
			}

			// If it's still stable start reconnecting
			if (currentlyStable == 1) {
				if (xQueueSend(reconnectOrShedQ, RECONNECT, 0) == pdPASS){
					// printf("YESS"); // used to confirm that it's being sent
				}
				printf("R");
			}
		} else {
			// else wait for 5 ms cause not being too greedy
			vTaskDelay(pdMS_TO_TICKS(5));
		}

	}
}
