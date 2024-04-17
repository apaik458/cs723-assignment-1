/*
 * queues.h
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#ifndef QUEUES_H_
#define QUEUES_H_

// Queue includes
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define RECONNECT 0 // A reconnect message in reconnectOrShedQ
#define SHED 1 // A shed message in reconnectOrShedQ

// Definition of Message Queue
#define MSG_QUEUE_SIZE 30
QueueHandle_t newFreqQ;
QueueHandle_t isStableQ;
QueueHandle_t reconnectOrShedQ;
QueueHandle_t freqMeasureQ;
QueueHandle_t keyPressQ;
QueueHandle_t switchStateQ;
QueueHandle_t maintenanceQ;
QueueHandle_t loadCtrlQ;
QueueHandle_t latencyQ;

SemaphoreHandle_t shared_resource_sem; // Definition of Semaphore

int initOSDataStructs(void);

#endif /* QUEUES_H_ */
