/*
 * vgaTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#include "../freertos/FreeRTOS.h"
#include "../queues.h"
#include "../globals.h"
#include "defines.h"

#include <stdio.h>

#include "vgaTask.h"
#include <stdio.h>

#define KEY_S 0x1b
#define KEY_L 0x4b
#define KEY_T 0x2c

void vgaTask(void *pvParameters) {
	//initialize VGA controllers
	alt_up_pixel_buffer_dma_dev *pixel_buf;
	pixel_buf = alt_up_pixel_buffer_dma_open_dev(VIDEO_PIXEL_BUFFER_DMA_NAME);

	if (pixel_buf == NULL) {
		printf("can't find pixel buffer device\n");
	}

	alt_up_pixel_buffer_dma_clear_screen(pixel_buf, 0);

	alt_up_char_buffer_dev *char_buf;
	char_buf = alt_up_char_buffer_open_dev(
			"/dev/video_character_buffer_with_dma");

	if (char_buf == NULL) {
		printf("can't find char buffer device\n");
	}

	alt_up_char_buffer_clear(char_buf);

	//Set up plot axes
	alt_up_pixel_buffer_dma_draw_hline(pixel_buf, 100, 590, 200,
			((0x3ff << 20) + (0x3ff << 10) + (0x3ff)), 0);
	alt_up_pixel_buffer_dma_draw_hline(pixel_buf, 100, 590, 300,
			((0x3ff << 20) + (0x3ff << 10) + (0x3ff)), 0);
	alt_up_pixel_buffer_dma_draw_vline(pixel_buf, 100, 50, 200,
			((0x3ff << 20) + (0x3ff << 10) + (0x3ff)), 0);
	alt_up_pixel_buffer_dma_draw_vline(pixel_buf, 100, 220, 300,
			((0x3ff << 20) + (0x3ff << 10) + (0x3ff)), 0);

	alt_up_char_buffer_string(char_buf, "Frequency(Hz)", 4, 4);
	alt_up_char_buffer_string(char_buf, "52", 10, 7);
	alt_up_char_buffer_string(char_buf, "50", 10, 12);
	alt_up_char_buffer_string(char_buf, "48", 10, 17);
	alt_up_char_buffer_string(char_buf, "46", 10, 22);

	alt_up_char_buffer_string(char_buf, "df/dt(Hz/s)", 4, 26);
	alt_up_char_buffer_string(char_buf, "60", 10, 28);
	alt_up_char_buffer_string(char_buf, "30", 10, 30);
	alt_up_char_buffer_string(char_buf, "0", 10, 32);
	alt_up_char_buffer_string(char_buf, "-30", 9, 34);
	alt_up_char_buffer_string(char_buf, "-60", 9, 36);

	alt_up_char_buffer_string(char_buf, "'s' - status", 5, 57);
	alt_up_char_buffer_string(char_buf, "'l' - loads", 34, 57);
	alt_up_char_buffer_string(char_buf, "'t' - time", 63, 57);

	double freq[100], dfreq[100];
	int i = 99, j = 0;
	Line line_freq, line_roc;

	unsigned char prevKeyPress = KEY_S;
	unsigned char keyPress = prevKeyPress;

	unsigned int prevLoads = 0;
	unsigned int loads = prevLoads;

	unsigned int prevMaintenanceState = 2;

	unsigned int statusChanged = 0;
	unsigned int latencyList[5] = { 0, 0, 0, 0, 0 };
	unsigned int minLatency = 200;
	unsigned int maxLatency = 0;
	float averageLatency = 0;
	unsigned int systemUptime = 0;

	unsigned int averageTotal = 0;
	unsigned int receivedLatency = 0;

	while (1) {
		systemUptime = xTaskGetTickCount() / 1000;
		//recieve load data from queue
		while (uxQueueMessagesWaiting(latencyQ) != 0) {
			statusChanged = 1;
			unsigned int tempLatency;
			xQueueReceive(latencyQ, &tempLatency, 0);

			// Update Latency List
			for (int i = 4; i > 0; i--) {
				latencyList[i] = latencyList[i - 1];
			}
			latencyList[0] = tempLatency;

			// Update min, max, avg
			if (tempLatency < minLatency && tempLatency > 0) {
				minLatency = tempLatency;
			}
			if (tempLatency > maxLatency){
				maxLatency = tempLatency;
			}

			averageTotal += tempLatency;
			receivedLatency++;
			averageLatency = (float)averageTotal / receivedLatency;
		}

		//receive frequency data from queue
		while (uxQueueMessagesWaiting(freqMeasureQ) != 0) {
			xQueueReceive(freqMeasureQ, freq + i, 0);
			xQueueReceive(keyPressQ, &keyPress, 0);
			xQueueReceive(vgaLoadsQ, &loads, 0);

			//calculate frequency RoC
			if (i == 0) {
				dfreq[0] = (freq[0] - freq[99]) * 2.0 * freq[0] * freq[99]
						/ (freq[0] + freq[99]);
			} else {
				dfreq[i] = (freq[i] - freq[i - 1]) * 2.0 * freq[i] * freq[i - 1]
						/ (freq[i] + freq[i - 1]);
			}

			if (dfreq[i] > 100.0) {
				dfreq[i] = 100.0;
			}

			if (keyPress != prevKeyPress || loads != prevLoads || statusChanged
					|| isMaintenanceState != prevMaintenanceState) {
				prevKeyPress = keyPress;
				prevLoads = loads;
				statusChanged = 0;
				prevMaintenanceState = isMaintenanceState;

				char temp[60];

				switch (keyPress) {
				case KEY_S:
					alt_up_char_buffer_clear(char_buf);
					sprintf(temp, "Frequency threshold: %.2f",
					THRESHOLD_FREQUENCY);
					alt_up_char_buffer_string(char_buf, temp, 10, 41);
					sprintf(temp, "RoC threshold: %.2f",
					THRESHOLD_ROC_FREQUENCY);
					alt_up_char_buffer_string(char_buf, temp, 10, 45);
					alt_up_char_buffer_string(char_buf, "System status", 50,
							41);

					// Display system state
					if (isMaintenanceState) {
						alt_up_char_buffer_string(char_buf, "Maintenance", 52,
								45);
					} else if (freq[i] < THRESHOLD_FREQUENCY
							|| dfreq[i] < THRESHOLD_ROC_FREQUENCY) {
						alt_up_char_buffer_string(char_buf, "Unstable", 53, 45);
					} else {
						alt_up_char_buffer_string(char_buf, "Stable", 54, 45);
					}
					break;
				case KEY_L:
					alt_up_char_buffer_clear(char_buf);
					alt_up_char_buffer_string(char_buf,
							loads & 0b1 ? "Load 1: Active" : "Load 1: Shed/Off", 10,
							41);
					alt_up_char_buffer_string(char_buf,
							loads & 0b10 ? "Load 2: Active" : "Load 2: Shed/Off",
							10, 43);
					alt_up_char_buffer_string(char_buf,
							loads & 0b100 ? "Load 3: Active" : "Load 3: Shed/Off",
							10, 45);
					alt_up_char_buffer_string(char_buf,
							loads & 0b1000 ? "Load 4: Active" : "Load 4: Shed/Off",
							10, 47);
					alt_up_char_buffer_string(char_buf,
							loads & 0b10000 ? "Load 5: Active" : "Load 5: Shed/Off",
							10, 49);
					break;
				case KEY_T:
					alt_up_char_buffer_clear(char_buf);

					sprintf(temp, "5 most recent measurements: %d %d %d %d %d",
							latencyList[0], latencyList[1], latencyList[2],
							latencyList[3], latencyList[4]);
					alt_up_char_buffer_string(char_buf, temp, 10, 41);

					sprintf(temp, "Minimum time taken: %d", minLatency);
					alt_up_char_buffer_string(char_buf, temp, 10, 43);

					sprintf(temp, "Maximum time taken: %d", maxLatency);
					alt_up_char_buffer_string(char_buf, temp, 10, 45);

					sprintf(temp, "Average time taken: %.2f", averageLatency);
					alt_up_char_buffer_string(char_buf, temp, 10, 47);

					sprintf(temp, "Total active system time: %d", systemUptime);
					alt_up_char_buffer_string(char_buf, temp, 10, 49);
					break;
				}

				// Print Frequency Axis
				alt_up_char_buffer_string(char_buf, "Frequency(Hz)", 4, 4);
				alt_up_char_buffer_string(char_buf, "52", 10, 7);
				alt_up_char_buffer_string(char_buf, "50", 10, 12);
				alt_up_char_buffer_string(char_buf, "48", 10, 17);
				alt_up_char_buffer_string(char_buf, "46", 10, 22);

				// Print Frequency RoC Axis
				alt_up_char_buffer_string(char_buf, "df/dt(Hz/s)", 4, 26);
				alt_up_char_buffer_string(char_buf, "60", 10, 28);
				alt_up_char_buffer_string(char_buf, "30", 10, 30);
				alt_up_char_buffer_string(char_buf, "0", 10, 32);
				alt_up_char_buffer_string(char_buf, "-30", 9, 34);
				alt_up_char_buffer_string(char_buf, "-60", 9, 36);

				// Print Menu
				alt_up_char_buffer_string(char_buf, "'s' - status", 5, 57);
				alt_up_char_buffer_string(char_buf, "'l' - loads", 34, 57);
				alt_up_char_buffer_string(char_buf, "'t' - time", 63, 57);
			}
			i = ++i % 100; //point to the next data (oldest) to be overwritten
		}

		//clear old graph to draw new graph
		alt_up_pixel_buffer_dma_draw_box(pixel_buf, 101, 0, 639, 199, 0, 0);
		alt_up_pixel_buffer_dma_draw_box(pixel_buf, 101, 201, 639, 299, 0, 0);

		for (j = 0; j < 99; ++j) { //i here points to the oldest data, j loops through all the data to be drawn on VGA
			if (((int) (freq[(i + j) % 100]) > MIN_FREQ)
					&& ((int) (freq[(i + j + 1) % 100]) > MIN_FREQ)) {
				//Calculate coordinates of the two data points to draw a line in between
				//Frequency plot
				line_freq.x1 = FREQPLT_ORI_X + FREQPLT_GRID_SIZE_X * j;
				line_freq.y1 = (int) (FREQPLT_ORI_Y
						- FREQPLT_FREQ_RES * (freq[(i + j) % 100] - MIN_FREQ));

				line_freq.x2 = FREQPLT_ORI_X + FREQPLT_GRID_SIZE_X * (j + 1);
				line_freq.y2 = (int) (FREQPLT_ORI_Y
						- FREQPLT_FREQ_RES
								* (freq[(i + j + 1) % 100] - MIN_FREQ));

				//Frequency RoC plot
				line_roc.x1 = ROCPLT_ORI_X + ROCPLT_GRID_SIZE_X * j;
				line_roc.y1 = (int) (ROCPLT_ORI_Y
						- ROCPLT_ROC_RES * dfreq[(i + j) % 100]);

				line_roc.x2 = ROCPLT_ORI_X + ROCPLT_GRID_SIZE_X * (j + 1);
				line_roc.y2 = (int) (ROCPLT_ORI_Y
						- ROCPLT_ROC_RES * dfreq[(i + j + 1) % 100]);

				//Draw
				alt_up_pixel_buffer_dma_draw_line(pixel_buf, line_freq.x1,
						line_freq.y1, line_freq.x2, line_freq.y2, 0x3ff << 0,
						0);
				alt_up_pixel_buffer_dma_draw_line(pixel_buf, line_roc.x1,
						line_roc.y1, line_roc.x2, line_roc.y2, 0x3ff << 0, 0);
			}
		}

		vTaskDelay(10);
	}
}
