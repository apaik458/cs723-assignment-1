/*
 * defines.h
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#ifndef DEFINES_H_
#define DEFINES_H_

// Scheduler includes
#include "../freertos/FreeRTOS.h"
#include "../freertos/task.h"

// Definition of Task Stacks
#define TASK_STACKSIZE ( 4096 )

// Definition of Task Priorities
#define STABILITYMONITORTASK_PRIORITY 1
#define SHOULDSHEDTASK_PRIORITY       1
#define SHOULDRECONNECTTASK_PRIORITY  1
#define LOADCTRLTASK_PRIORITY         1
#define SWITCHPOLLINGTASK_PRIORITY    1
#define LEDTASK_PRIORITY              ( tskIDLE_PRIORITY + 1)
#define VGATASK_PRIORITY              1

/* The parameters passed to the reg test tasks.  This is just done to check
 the parameter passing mechanism is working correctly. */
#define mainREG_TEST_1_PARAMETER ((void *) 0x12345678)
#define mainREG_TEST_PRIORITY    (tskIDLE_PRIORITY + 1)

TaskHandle_t xHandle; // Used to delete a tasks

#endif /* DEFINES_H_ */
