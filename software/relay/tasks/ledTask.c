/*
 * ledTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#include "ledTask.h"

void ledTask(void *pvParameters)
{
	while (1)
	{
		printf("ledTask\n");
		vTaskDelay(1000);
	}
}
