/*
 * ledTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#include <stdio.h>

#include <system.h>
#include <altera_avalon_pio_regs.h>

#include "../queues.h"

#include "ledTask.h"

void ledTask(void *pvParameters)
{
	unsigned int loadCtrl = 0;

	while (1)
	{
		if (xQueueReceive(loadCtrlQ, &loadCtrl, 0) == pdPASS) {
			// printf("ledTask\n");
		    IOWR_ALTERA_AVALON_PIO_DATA(GREEN_LEDS_BASE, loadCtrl);
		    IOWR_ALTERA_AVALON_PIO_DATA(RED_LEDS_BASE, ~loadCtrl);
		}
//		vTaskDelay(1);
	}
}
