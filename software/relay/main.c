// Standard includes
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// System includes
#include "system.h"
#include <altera_avalon_pio_regs.h>
#include "sys/alt_irq.h"
#include "io.h"
#include "altera_up_avalon_ps2.h"
#include "altera_up_ps2_keyboard.h"

// Source includes
#include "tasks.h"
#include "queues.h"

FILE *lcd;
static unsigned char esc = 0x1b;

void FreqAnalyserISR() {
	#define SAMPLING_FREQ 16000.0
	double temp = SAMPLING_FREQ / (double)IORD(FREQUENCY_ANALYSER_BASE, 0);

	xQueueSendToBackFromISR(newFreqQ, &temp, pdFALSE);

	return;
}

void ps2_isr(void* context, alt_u32 id)
{
	char ascii;
	int status = 0;
	unsigned char key = 0;
	KB_CODE_TYPE decode_mode;
	status = decode_scancode(context, &decode_mode, &key, &ascii);

	if (status == 0) //success
	{
		// print out the result
		switch (decode_mode)
		{
		  case KB_ASCII_MAKE_CODE:
			fprintf(lcd, "ASCII: %x\n", key);
			xQueueSendToBackFromISR(keyPressQ, &key, pdFALSE);
			break;
		  case KB_LONG_BINARY_MAKE_CODE:
			// do nothing
		  case KB_BINARY_MAKE_CODE:
			fprintf(lcd, "MAKE CODE: %x\n", key);
			xQueueSendToBackFromISR(keyPressQ, &key, pdFALSE);
			break;
		  case KB_BREAK_CODE:
			// do nothing
		  default:
			fprintf(lcd, "DEFAULT: %x\n", key);
			xQueueSendToBackFromISR(keyPressQ, &key, pdFALSE);
			break;
		}

		IOWR(SEVEN_SEG_BASE, 0, key);
	}
}

int main()
{
	lcd = fopen(CHARACTER_LCD_NAME, "w");
	alt_up_ps2_dev * ps2_device = alt_up_ps2_open_dev(PS2_NAME);

	if(ps2_device == NULL){
		fprintf(lcd, "%c%scan't find PS/2 device\n", esc, "[2J"); //esc character (0x1b) followed by "[2J]" clears the screen
		return 1;
	}

	fprintf(lcd, "%c%sInitialised\n", esc, "[2J"); //esc character (0x1b) followed by "[2J]" clears the screen
	alt_up_ps2_clear_fifo(ps2_device);

	alt_irq_register(PS2_IRQ, ps2_device, ps2_isr);
	// register the PS/2 interrupt
	IOWR_8DIRECT(PS2_BASE, 4, 1);

	initOSDataStructs();
	initCreateTasks();
	alt_irq_register(FREQUENCY_ANALYSER_IRQ, 0, FreqAnalyserISR);
	vTaskStartScheduler();

	while(1)
	{
		;
	}

	return 0;
}
