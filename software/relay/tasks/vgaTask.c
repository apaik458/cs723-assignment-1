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

	uint16_t latencyList[5] = {0,0,0,0,0};
	uint16_t minLatency = 200;
	uint16_t maxLatency = 0;
	uint32_t averageTotal = 0;
	uint16_t averageLatency = 0;
	uint16_t receivedLatency = 0;
	uint16_t systemUptime = 0;


	while (1) {
		systemUptime = xTaskGetTickCount()/1000;
		//recieve load data from queue
		while (uxQueueMessagesWaiting(latencyQ) != 0) {
			unsigned int tempLatency;
			xQueueReceive(latencyQ, &tempLatency, 0);

			// Update Latency List
			printf("Latencies: ");
			for (int8_t i = 4; i < 1; i--){
				latencyList[i] = latencyList[i-1];
				prinf(" %d,", latencyList[i]);
			}
			latencyList[0] = tempLatency;
			prinf(" %d\n", latencyList[0]);


			// Update min, max, avg
			if (tempLatency < minLatency)
				minLatency = tempLatency;
			if (tempLatency > maxLatency)
				maxLatency = tempLatency;

			averageTotal += tempLatency;
			receivedLatency++;   
			averageLatency = averageTotal/receivedLatency;
		}

		//receive frequency data from queue
		while (uxQueueMessagesWaiting(freqMeasureQ) != 0) {
			xQueueReceive(freqMeasureQ, freq + i, 0);
			xQueueReceive(keyPressQ, &keyPress, 0);
			xQueueReceive(vgaLoadsQ, &loads, 0);

			if (keyPress != prevKeyPress || loads != prevLoads) {
				prevKeyPress = keyPress;
				prevLoads = loads;

				switch (keyPress) {
				case KEY_S:
					alt_up_char_buffer_clear(char_buf);
					char temp[30];
					sprintf(temp, "Frequency threshold: %.2f",
					THRESHOLD_FREQUENCY);
					alt_up_char_buffer_string(char_buf, temp, 10, 41);
					sprintf(temp, "RoC threshold: %.2f",
					THRESHOLD_ROC_FREQUENCY);
					alt_up_char_buffer_string(char_buf, temp, 10, 45);
					alt_up_char_buffer_string(char_buf, "System status", 50,
							41);
					break;
				case KEY_L:
					alt_up_char_buffer_clear(char_buf);
					alt_up_char_buffer_string(char_buf,
							loads & 0b1 ? "Load 1: Active" : "Load 1: Shed", 10,
							41);
					alt_up_char_buffer_string(char_buf,
							loads & 0b10 ? "Load 2: Active" : "Load 2: Shed",
							10, 43);
					alt_up_char_buffer_string(char_buf,
							loads & 0b100 ? "Load 3: Active" : "Load 3: Shed",
							10, 45);
					alt_up_char_buffer_string(char_buf,
							loads & 0b1000 ? "Load 4: Active" : "Load 4: Shed",
							10, 47);
					alt_up_char_buffer_string(char_buf,
							loads & 0b10000 ? "Load 5: Active" : "Load 5: Shed",
							10, 49);
					break;
				case KEY_T:
					alt_up_char_buffer_clear(char_buf);
					alt_up_char_buffer_string(char_buf,
							"5 most recent measurements: ", 10, 41);
					alt_up_char_buffer_string(char_buf, "Minimum time taken: ",
							10, 43);
					alt_up_char_buffer_string(char_buf, "Maximum time taken: ",
							10, 45);
					alt_up_char_buffer_string(char_buf, "Average time taken: ",
							10, 47);
					alt_up_char_buffer_string(char_buf,
							"Total active system time: ", 10, 49);
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
