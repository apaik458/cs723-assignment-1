/*
 * queues.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#include "queues.h"

// This function creates the message queues and semaphores
int initOSDataStructs(void)
{
	newFreqQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	freqMeasureQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	isStableQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	reconnectOrShedQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	switchStateQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	keyPressQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	maintenanceQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	loadCtrlQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	shared_resource_sem = xSemaphoreCreateCounting(9999, 1);
	return 0;
}
