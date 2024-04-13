/*
 * loadCtrlTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#include "loadCtrlTask.h"

void loadCtrlTask(void *pvParameters)
{
	while (1)
	{
		printf("loadCtrlTask\n");
		vTaskDelay(1500);
	}
}
