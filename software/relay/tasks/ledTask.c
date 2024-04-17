/*
 * ledTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

// Standard library includes
#include <stdio.h>

// System includes
#include <system.h>
#include <altera_avalon_pio_regs.h>

// Source includes
#include "../queues.h"
#include "defines.h"

#include "ledTask.h"

/**
 * Use red LEDs to display loads which are connected, and green to show loads
 * which are shed.
 */
void ledTask(void *pvParameters) {
	unsigned int loadCtrl = 0;

	while (1) {
		if (xQueueReceive(loadCtrlQ, &loadCtrl, 0) == pdPASS) {
			IOWR_ALTERA_AVALON_PIO_DATA(RED_LEDS_BASE, loadCtrl);
			IOWR_ALTERA_AVALON_PIO_DATA(GREEN_LEDS_BASE,
					(~loadCtrl & LOAD_MASK));
		}
	}
}
