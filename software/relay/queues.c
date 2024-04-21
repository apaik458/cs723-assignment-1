/*
 * queues.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#include "queues.h"

/**
 * Create message queues and semaphores.
 */
void initOSDataStructs() {
	// Queues
	newFreqQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(double));
	freqMeasureQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(double));
	reconnectOrShedQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(unsigned int));
	switchStateQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(unsigned int));
	keyPressQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(unsigned int));
	loadCtrlQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(unsigned int));
	vgaLoadsQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(unsigned int));
	latencyQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(unsigned int));

	// Semaphores
	xisStableMutex = xSemaphoreCreateMutex();
	xfirstTickMutex = xSemaphoreCreateMutex();
	xfirstTick = -1;
}
