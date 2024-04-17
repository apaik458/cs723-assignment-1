/*
 * loadCtrlTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

// Standard library includes
#include <stdio.h>

// OS includes
#include "../freertos/FreeRTOS.h"
#include "../freertos/task.h"

// Source includes
#include "../globals.h"
#include "../queues.h"
#include "../globals.h"

#include "loadCtrlTask.h"

#define UNSIGNED_INT_SIZE sizeof(unsigned int) * 8

/*
 * Get the position of the most significant high bit.
 */
int mostSignificantHighBit(unsigned int bits) {
	for (int i = UNSIGNED_INT_SIZE - 1; i >= 0; i--) {
		if (bits & (1 << i))
			return i;
	}

	return 0;
}

/*
 * Get the position of the least significant high bit.
 */
int leastSignificantHighBit(unsigned int bits) {
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
	unsigned int prevIsMaintenanceState = 0;

	unsigned int prevSwitchState = 0;
	unsigned int switchState = prevSwitchState;

	unsigned int prevShedLoads = 0; // Start with no loads shed
	unsigned int shedLoads = prevShedLoads;

	while (1) {
		xQueueReceive(switchStateQ, &switchState, 0); // Check for switch state update

		if (isMaintenanceState) {
			// Maintenance state, obey switches
			if (isMaintenanceState != prevIsMaintenanceState
					|| switchState != prevSwitchState) {
				// Must update load state
				if (xQueueSend(loadCtrlQ, &switchState, 0) == pdPASS) {
					// Load states updated, update previous values so another loadCtrlQ isn't sent
					prevIsMaintenanceState = isMaintenanceState;
					prevSwitchState = switchState;

					prevShedLoads = 0;
					shedLoads = prevShedLoads;
				}
			}
		} else {
			// Normal state, obey reconnect or shed messages
			unsigned int connectedLoads = switchState & ~shedLoads;

			unsigned int reconnectOrShed;
			if (xQueueReceive(reconnectOrShedQ, &reconnectOrShed, 0) == pdPASS) {
				switch (reconnectOrShed) {
				case RECONNECT:
					// Reconnect the highest priority load
					if (shedLoads) // There are loads available to shed
						shedLoads &= ~(1 << mostSignificantHighBit(shedLoads));
					break;
				case SHED:
					// Shed the lowest priority load
					if (connectedLoads) // There are loads available to connect
						shedLoads |= (1
								<< leastSignificantHighBit(connectedLoads));

					// Get tick where instability is detected
					TickType_t instabilityTick = 0;
					if (xSemaphoreTake(xfirstTickMutex, portMAX_DELAY) == pdPASS) {
						instabilityTick = xfirstTick; // Set the global variable
						xfirstTick = -1;
						xSemaphoreGive(xfirstTickMutex);
					}

					// if it's the first time shedding
					if (instabilityTick != -1) {
						uint16_t reactionTimeMS =  ((uint16_t)xTaskGetTickCount() - (uint16_t)instabilityTick)/(float)configTICK_RATE_HZ *1000;
						printf("Reaction Time: %dms | %d\n", reactionTimeMS, (uint16_t)instabilityTick);
						if (xQueueSend(latencyQ, &reactionTimeMS, 0) != pdPASS) {
							// Send Failed
							printf("ERROR: latencyQ Send Fail");
						}

					}

					break;
				}
			}

			if (shedLoads != prevShedLoads || switchState != prevSwitchState) {
				// Must update load state
				connectedLoads = switchState & ~shedLoads & ((0b1 << 5) - 1);

				if (xQueueSend(loadCtrlQ, &connectedLoads, 0) == pdPASS) {
					// Load states updated, update previous values so another loadCtrlQ isn't sent
					prevShedLoads = shedLoads;
				}
			}
		}
	}
}
