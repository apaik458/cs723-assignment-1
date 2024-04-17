/*
 * vgaTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#include "../freertos/FreeRTOS.h"
#include "../queues.h"
#include "../globals.h"

#include <stdio.h>

#include "vgaTask.h"


void vgaTask(void *pvParameters)
{
	double freqQData[2];
	for (;;){
		if (xQueueReceive(freqMeasureQ, &freqQData , 0) == pdPASS) {
//			printf("VGA Received F: %f, ROC: %f\n", freqQData[0], freqQData[1]);
			;
		}
	}
}
