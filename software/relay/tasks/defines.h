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
#define LOADCTRLTASK_PRIORITY         1
#define LEDTASK_PRIORITY              1
#define STABILITYMONITORTASK_PRIORITY 2
#define SHOULDSHEDTASK_PRIORITY       2
#define SHOULDRECONNECTTASK_PRIORITY  2
#define SWITCHPOLLINGTASK_PRIORITY    3
#define VGATASK_PRIORITY              4

TaskHandle_t xHandle; // Used to delete a tasks
TaskHandle_t PRVGADraw;

// Define thresholds
#define THRESHOLD_FREQUENCY 49.0
#define THRESHOLD_ROC_FREQUENCY -15.0

#define LOAD_MASK ((0b1 << 5) - 1)

#endif /* DEFINES_H_ */
