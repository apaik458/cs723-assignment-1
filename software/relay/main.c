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

// Source includes
#include "tasks.h"
#include "queues.h"

void FreqAnalyserISR() {
	#define SAMPLING_FREQ 16000.0
	double temp = SAMPLING_FREQ / (double)IORD(FREQUENCY_ANALYSER_BASE, 0);

	xQueueSendToBackFromISR(newFreqQ, &temp, pdFALSE);

	return;
}

int main()
{
	initOSDataStructs();
	initCreateTasks();
	alt_irq_register(FREQUENCY_ANALYSER_IRQ, 0, FreqAnalyserISR);
	vTaskStartScheduler();

	while (1) {
		;
	}

	return 0;
}
