/*
 * vgaTask.c
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#include "vgaTask.h"
#include <stdio.h>
void vgaTask(void *pvParameters) {
	//initialize VGA controllers
	alt_up_pixel_buffer_dma_dev *pixel_buf;
	pixel_buf = alt_up_pixel_buffer_dma_open_dev(VIDEO_PIXEL_BUFFER_DMA_NAME);

	if (pixel_buf == NULL) {
		printf("can't find pixel buffer device\n");
	}

	alt_up_pixel_buffer_dma_clear_screen(pixel_buf, 0);

	alt_up_char_buffer_dev *char_buf;
	char_buf = alt_up_char_buffer_open_dev("/dev/video_character_buffer_with_dma");

	if (char_buf == NULL) {
		printf("can't find char buffer device\n");
	}

	alt_up_char_buffer_clear(char_buf);

	//Set up plot axes
	alt_up_pixel_buffer_dma_draw_hline(pixel_buf, 100, 590, 200, ((0x3ff << 20) + (0x3ff << 10) + (0x3ff)), 0);
	alt_up_pixel_buffer_dma_draw_hline(pixel_buf, 100, 590, 300, ((0x3ff << 20) + (0x3ff << 10) + (0x3ff)), 0);
	alt_up_pixel_buffer_dma_draw_vline(pixel_buf, 100, 50, 200, ((0x3ff << 20) + (0x3ff << 10) + (0x3ff)), 0);
	alt_up_pixel_buffer_dma_draw_vline(pixel_buf, 100, 220, 300, ((0x3ff << 20) + (0x3ff << 10) + (0x3ff)), 0);

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

	alt_up_char_buffer_string(char_buf, "Lower threshold: ", 10, 41);
	alt_up_char_buffer_string(char_buf, "RoC threshold: ", 10, 45);
	alt_up_char_buffer_string(char_buf, "System status", 50, 41);
	alt_up_char_buffer_string(char_buf, "Stable", 54, 45);

	double freq[100], dfreq[100];
	int i = 99, j = 0;
	Line line_freq, line_roc;

	unsigned char keyPress;

	while(1) {
		//receive key press data from queue
//		while (uxQueueMessagesWaiting(keyPressQ) != 0) {
//			xQueueReceive(keyPressQ, &keyPress, 0);
//			alt_up_char_buffer_string(char_buf, (char*)keyPress, 7, 54);
//		}

		//receive frequency data from queue
		while (uxQueueMessagesWaiting(newFreqQ) != 0) {
			xQueueReceive(newFreqQ, freq+i, 0);

			if (uxQueueMessagesWaiting(keyPressQ)) {
				xQueueReceive(keyPressQ, &keyPress, 0);

				if (keyPress == 0x1b) { // 's'
					alt_up_char_buffer_clear(char_buf);
					alt_up_char_buffer_string(char_buf, "Lower threshold: 49.0", 10, 41);
					alt_up_char_buffer_string(char_buf, "RoC threshold: -0.5", 10, 45);
					alt_up_char_buffer_string(char_buf, "System status", 50, 41);

					alt_up_char_buffer_string(char_buf, "Stable", 54, 45);

//					if (maintenance) {
//						alt_up_char_buffer_string(char_buf, "Maintenance", 54, 45);
//					} else if (freq < freq_threshold || freq_roc < roc_threshold) {
//						alt_up_char_buffer_string(char_buf, "Unstable", 54, 45);
//					} else {
//						alt_up_char_buffer_string(char_buf, "Stable", 54, 45);
//					}
				} else if (keyPress == 0x4b) { // 'l'
					alt_up_char_buffer_clear(char_buf);
					alt_up_char_buffer_string(char_buf, "Load 1: Active", 10, 41);
					alt_up_char_buffer_string(char_buf, "Load 2: Active", 10, 43);
					alt_up_char_buffer_string(char_buf, "Load 3: Active", 10, 45);
					alt_up_char_buffer_string(char_buf, "Load 4: Active", 10, 47);
					alt_up_char_buffer_string(char_buf, "Load 5: Active", 10, 49);
				} else if (keyPress == 0x2c) { // 't'
					alt_up_char_buffer_clear(char_buf);
					alt_up_char_buffer_string(char_buf, "5 most recent measurements: ", 10, 41);
					alt_up_char_buffer_string(char_buf, "Minimum time taken: ", 10, 43);
					alt_up_char_buffer_string(char_buf, "Maximum time taken: ", 10, 45);
					alt_up_char_buffer_string(char_buf, "Average time taken: ", 10, 47);
					alt_up_char_buffer_string(char_buf, "Total active system time: ", 10, 49);
				}

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
			}

			//calculate frequency RoC
			if (i==0) {
				dfreq[0] = (freq[0]-freq[99]) * 2.0 * freq[0] * freq[99] / (freq[0]+freq[99]);
			} else {
				dfreq[i] = (freq[i]-freq[i-1]) * 2.0 * freq[i]* freq[i-1] / (freq[i]+freq[i-1]);
			}

			if (dfreq[i] > 100.0) {
				dfreq[i] = 100.0;
			}

			i =	++i%100; //point to the next data (oldest) to be overwritten
		}

		//clear old graph to draw new graph
		alt_up_pixel_buffer_dma_draw_box(pixel_buf, 101, 0, 639, 199, 0, 0);
		alt_up_pixel_buffer_dma_draw_box(pixel_buf, 101, 201, 639, 299, 0, 0);

		for (j = 0; j < 99; ++j) { //i here points to the oldest data, j loops through all the data to be drawn on VGA
			if (((int)(freq[(i+j)%100]) > MIN_FREQ) && ((int)(freq[(i+j+1)%100]) > MIN_FREQ)) {
				//Calculate coordinates of the two data points to draw a line in between
				//Frequency plot
				line_freq.x1 = FREQPLT_ORI_X + FREQPLT_GRID_SIZE_X * j;
				line_freq.y1 = (int)(FREQPLT_ORI_Y - FREQPLT_FREQ_RES * (freq[(i+j)%100] - MIN_FREQ));

				line_freq.x2 = FREQPLT_ORI_X + FREQPLT_GRID_SIZE_X * (j + 1);
				line_freq.y2 = (int)(FREQPLT_ORI_Y - FREQPLT_FREQ_RES * (freq[(i+j+1)%100] - MIN_FREQ));

				//Frequency RoC plot
				line_roc.x1 = ROCPLT_ORI_X + ROCPLT_GRID_SIZE_X * j;
				line_roc.y1 = (int)(ROCPLT_ORI_Y - ROCPLT_ROC_RES * dfreq[(i+j)%100]);

				line_roc.x2 = ROCPLT_ORI_X + ROCPLT_GRID_SIZE_X * (j + 1);
				line_roc.y2 = (int)(ROCPLT_ORI_Y - ROCPLT_ROC_RES * dfreq[(i+j+1)%100]);

				//Draw
				alt_up_pixel_buffer_dma_draw_line(pixel_buf, line_freq.x1, line_freq.y1, line_freq.x2, line_freq.y2, 0x3ff << 0, 0);
				alt_up_pixel_buffer_dma_draw_line(pixel_buf, line_roc.x1, line_roc.y1, line_roc.x2, line_roc.y2, 0x3ff << 0, 0);
			}
		}

		vTaskDelay(10);
	}
}
