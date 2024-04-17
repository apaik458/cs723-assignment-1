/*
 * switchPollingTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

// Standard library includes
#include <stdio.h>

// System includes
#include <system.h>
#include <altera_avalon_pio_regs.h>

// OS includes
#include "../freertos/FreeRTOS.h"
#include "../freertos/task.h"

// Source includes
#include "../queues.h"
#include "defines.h"

#include "switchPollingTask.h"

/*
 * Task to poll SW[4..0] for changes, and send a message to loadCtrlTask.
 */
void switchPollingTask(void *pvParameters) {
	unsigned int prevSwitchState = 0;
	unsigned int switchState = prevSwitchState;
	printf("switch\n");

	while (1) {
		switchState = IORD_ALTERA_AVALON_PIO_DATA(SLIDE_SWITCH_BASE) & LOAD_MASK;

		if (switchState != prevSwitchState) {
			// Switch changed, send state message
			printf("switch state %d\n", switchState);
			if (xQueueSend(switchStateQ, &switchState, 0) == pdPASS) {
				// Added to message queue
				prevSwitchState = switchState;
			}
		}

		vTaskDelay(20);
	}
}
