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

// Definition of Message Queue
#define MSG_QUEUE_SIZE 30
QueueHandle_t newFreqQ;
QueueHandle_t isStableQ;
QueueHandle_t shedQ;
QueueHandle_t reconnectQ;
QueueHandle_t freqMeasureQ;
QueueHandle_t keyPressQ;
QueueHandle_t switchStateQ;
QueueHandle_t maintenanceQ;
QueueHandle_t loadCtrlQ;

SemaphoreHandle_t shared_resource_sem; // Definition of Semaphore

int initOSDataStructs(void);

#endif /* QUEUES_H_ */
