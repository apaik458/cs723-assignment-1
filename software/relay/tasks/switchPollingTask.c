/*
 * switchPollingTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#include <system.h>
#include <altera_avalon_pio_regs.h>

#include "../freertos/FreeRTOS.h"
#include "../freertos/task.h"

#include "../queues.h"
#include "switchPollingTask.h"

/*
 * Task to pollsSW[4..0] for changes, and send a message to loadCtrlTask.
 */
void switchPollingTask(void *pvParameters) {
	unsigned int switchValue = 0;
	unsigned int prevSwitchValue = 0;

	while (1) {
		switchValue = IORD_ALTERA_AVALON_PIO_DATA(SLIDE_SWITCH_BASE) & LOAD_SWITCH_MASK;

		if (switchValue != prevSwitchValue) {
			// Switch changed, send state message
			if (xQueueSend(switchStateQ, (void *)&switchValue, 0) == pdPASS) {
				// Added to message queue
				prevSwitchValue = switchValue;
			}
		}

		vTaskDelay(100);
	}
}
