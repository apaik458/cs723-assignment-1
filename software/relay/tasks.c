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
	xTaskCreate(stabilityMonitorTask, "stabilityMonitorTask", TASK_STACKSIZE, NULL, 2/*STABILITYMONITORTASK_PRIORITY*/, NULL);
	xTaskCreate(shouldShedTask,       "shouldShedTask",       TASK_STACKSIZE, NULL, 2/*SHOULDSHEDTASK_PRIORITY*/, NULL);
	xTaskCreate(shouldReconnectTask,  "shouldReconnectTask",  TASK_STACKSIZE, NULL, 2/*SHOULDRECONNECTTASK_PRIORITY*/, NULL);
	xTaskCreate(loadCtrlTask,         "loadCtrlTask",         TASK_STACKSIZE, NULL, 2/*LOADCTRLTASK_PRIORITY*/, NULL);
	xTaskCreate(switchPollingTask,    "switchPollingTask",    TASK_STACKSIZE, NULL, 2/*SWITCHPOLLINGTASK_PRIORITY*/, NULL);
	xTaskCreate(ledTask,              "ledTask",              TASK_STACKSIZE, NULL, 2/*LEDTASK_PRIORITY*/, NULL);
	xTaskCreate(vgaTask, "vgaTask", configMINIMAL_STACK_SIZE, NULL, 3/*PRVGADraw_Task_P*/, &PRVGADraw);
}
