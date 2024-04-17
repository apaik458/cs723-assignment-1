/*
 * loadCtrlTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#include <stdio.h>

#include "../freertos/FreeRTOS.h"
#include "../freertos/task.h"

#include "../queues.h"

#include "loadCtrlTask.h"

#define UNSIGNED_INT_SIZE sizeof(unsigned int) * 8

/*
 * Get the position of the highest 1 bit.
 */
int getHighestPriorityBit(unsigned int bits) {
	for (int i = UNSIGNED_INT_SIZE - 1; i >= 0; i--) {
		if (bits & (1 << i))
			return i;
	}

	return 0;
}

/*
 * Get the position of the lowest 1 bit.
 */
int getLowestPriorityBit(unsigned int bits) {
	for (int i = 0; i <= UNSIGNED_INT_SIZE - 1; i++) {
		if (bits & (1 << i))
			return i;
	}

	return 0;
}

/*
 * Task to respond to maintenance state, switch, and reconnectOrShed changes.
 */
void loadCtrlTask(void *pvParameters) {
	unsigned int prevMaintenance = 0;
	unsigned int maintenance = prevMaintenance;

	unsigned int prevSwitchState = 0;
	unsigned int switchState = prevSwitchState;

	unsigned int prevShedLoads = 0; // Start with no loads shed
	unsigned int shedLoads = prevShedLoads;

	while (1) {
		xQueueReceive(switchStateQ, &switchState, 0); // Check for switch state update
		xQueueReceive(maintenanceQ, &maintenance, 0); // Check for maintenance state update

		if (maintenance) {
			printf("maintenance\n");
			// Maintenance state, obey switches
			if (maintenance != prevMaintenance
					|| switchState != prevSwitchState) {
				// Must update load state
				if (xQueueSend(loadCtrlQ, &switchState, 0) == pdPASS) {
					// Load states updated, update previous values so another loadCtrlQ isn't sent
					prevMaintenance = maintenance;
					prevSwitchState = switchState;

					shedLoads = ~switchState;
					prevShedLoads = shedLoads;
				}
			}
		} else {
			// Normal state, obey reconnect or shed messages
			unsigned int connectedLoads = switchState & ~shedLoads;

			unsigned int reconnectOrShed;
			if (xQueueReceive(reconnectOrShedQ, &reconnectOrShed, 0) == pdPASS) {
				printf("ros %d ss %d sl %d cl %d\n", reconnectOrShed, switchState, shedLoads, connectedLoads);
				switch (reconnectOrShed) {
				case RECONNECT:
					// Reconnect the highest priority load
					if (shedLoads) // There are loads available to shed
						shedLoads &= ~(1 << getHighestPriorityBit(shedLoads));
					break;
				case SHED:
					// Shed the lowest priority load
					if (connectedLoads) // There are loads available to connect
						shedLoads |= (1 << getLowestPriorityBit(connectedLoads));
					break;
				}
			}

			if (shedLoads != prevShedLoads) {
				// Must update load state
				connectedLoads = switchState & ~shedLoads;

				if (xQueueSend(loadCtrlQ, &connectedLoads, 0) == pdPASS) {
					// Load states updated, update previous values so another loadCtrlQ isn't sent
					prevShedLoads = shedLoads;
				}
			}
		}

		vTaskDelay(20);
	}
}
