/*
 * shouldReconnectTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

// Standard library includes
#include <stdio.h>
#include <stdint.h>

// OS includes
#include "../freertos/FreeRTOS.h"
#include "../freertos/task.h"

// Source includes
#include "../globals.h"
#include "../queues.h"

#include "shouldReconnectTask.h"

void shouldReconnectTask(void *pvParameters) {
	uint8_t currentlyStable = 0;
	for (;;) {
		if (!isMaintenanceState) {
			if (xSemaphoreTake(xisStableMutex, portMAX_DELAY) == pdPASS) {
				currentlyStable = xisStable;
				xSemaphoreGive(xisStableMutex);
			}

			if (currentlyStable == 1) {
				// Stable and should start timer
				// TODO: switch from delay to proper timer, this should be fine for the demo though
				vTaskDelay(pdMS_TO_TICKS(500));
				if (xSemaphoreTake(xisStableMutex, portMAX_DELAY) == pdPASS) {
					currentlyStable = xisStable;
					xSemaphoreGive(xisStableMutex);
				}

				// If it's still stable start reconnecting
				if (currentlyStable == 1) {
					unsigned int reconnect = RECONNECT;
					if (xQueueSend(reconnectOrShedQ, &reconnect, 0) == pdPASS) {
						// printf("YESS"); // used to confirm that it's being sent
					}
					//printf("R");
				}
			} else {
				// else wait for 5 ms cause not being too greedy
				vTaskDelay(pdMS_TO_TICKS(5));
			}
		}

		vTaskDelay(1);
	}
}
