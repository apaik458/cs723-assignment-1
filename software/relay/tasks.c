/*
 * tasks.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#include "tasks.h"

// This function creates the tasks used in this example
void initCreateTasks()
{
	/* The RegTest tasks as described at the top of this file. */
	xTaskCreate(stabilityMonitorTask, "stabilityMonitorTask", TASK_STACKSIZE, NULL, STABILITYMONITORTASK_PRIORITY, NULL);
	xTaskCreate(shouldShedTask,       "shouldShedTask",       TASK_STACKSIZE, NULL, SHOULDSHEDTASK_PRIORITY, NULL);
	xTaskCreate(shouldReconnectTask,  "shouldReconnectTask",  TASK_STACKSIZE, NULL, SHOULDRECONNECTTASK_PRIORITY, NULL);
	xTaskCreate(loadCtrlTask,         "loadCtrlTask",         TASK_STACKSIZE, NULL, LOADCTRLTASK_PRIORITY, NULL);
	xTaskCreate(switchPollingTask,    "switchPollingTask",    TASK_STACKSIZE, NULL, SWITCHPOLLINGTASK_PRIORITY, NULL);
	xTaskCreate(ledTask,              "ledTask",              TASK_STACKSIZE, NULL, LEDTASK_PRIORITY, NULL);
	xTaskCreate(vgaTask,              "vgaTask",              TASK_STACKSIZE, NULL, VGATASK_PRIORITY, NULL);
}