/*
 * shouldShedTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */
#include <stdio.h>
#include <stdint.h>

#include "shouldShedTask.h"
#include "../globals.h"
#include "../queues.h"

#include "../freertos/FreeRTOS.h"
#include "../freertos/task.h"



void shouldShedTask(void *pvParameters)
{
	uint8_t currentlyStable = 1;
	for (;;) {
		if (xSemaphoreTake(xisStableMutex, portMAX_DELAY) == pdPASS) {
			currentlyStable = xisStable;
			xSemaphoreGive(xisStableMutex);
		}

		if (currentlyStable == 0) {
			unsigned int shed = SHED;
			xQueueSend(reconnectOrShedQ, &shed, 0);
			printf("S");

			// Start timer do delay next (potential) shedding by 500ms
			// TODO: switch from delay to proper timer, this should be fine for the demo though
			vTaskDelay(pdMS_TO_TICKS(500));
		} else {
			// else wait for 5 ms cause not being too greedy
			vTaskDelay(pdMS_TO_TICKS(5));
		}

		vTaskDelay(1);
	}
}
