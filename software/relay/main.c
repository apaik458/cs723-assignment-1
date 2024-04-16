// Standard includes
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// System includes
#include "system.h"
#include <altera_avalon_pio_regs.h>
#include "sys/alt_irq.h"

// Source includes
#include "queues.h"
#include "tasks.h"
#include "globals.h"
#define SAMPLING_FREQ 16000.0

uint8_t xisStable = 1; // default to stable state
SemaphoreHandle_t xisStableMutex = NULL;

void FreqAnalyserISR(void* context, alt_u32 id)
{
	double temp = SAMPLING_FREQ / (double)IORD(FREQUENCY_ANALYSER_BASE, 0);
	xQueueSendToBackFromISR(newFreqQ, &temp, pdFALSE);
}

void ButtonISR(void* context, alt_u32 id)
{
	;
}

void KeyISR(void* context, alt_u32 id)
{
	;
}

/*
 * Create the demo tasks then start the scheduler.
 */
int main(int argc, char* argv[], char* envp[])
{
	int buttonValue = 0;

	// Create mutexs
	xisStableMutex = xSemaphoreCreateMutex();

	initOSDataStructs();
	initCreateTasks();

	// Register the IRQs
  	alt_irq_register(FREQUENCY_ANALYSER_IRQ, 0, FreqAnalyserISR);
	alt_irq_register(PUSH_BUTTON_IRQ, (void*)&buttonValue, ButtonISR);
	alt_irq_register(PS2_IRQ, (void*)&buttonValue, KeyISR);
	
	vTaskStartScheduler();

	for (;;);

	return 0;
}
