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

#define KEY_3(X) (X >> 2) & 0b1

void FreqAnalyserISR(void* context, alt_u32 id) {
	double temp = SAMPLING_FREQ / (double) IORD(FREQUENCY_ANALYSER_BASE, 0);
	xQueueSendFromISR(newFreqQ, &temp, pdFALSE);
}

void ButtonISR() {
	// Send maintenanceQ on KEY3 push
	unsigned int temp = KEY_3(IORD_ALTERA_AVALON_PIO_EDGE_CAP(PUSH_BUTTON_BASE));
	if (temp)
		isMaintenanceState = !isMaintenanceState;

	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PUSH_BUTTON_BASE, 0x7); // Clear edge capture register
}

void KeyISR(void* context, alt_u32 id) {
	;
}

/*
 * Create the demo tasks then start the scheduler.
 */
int main(int argc, char* argv[], char* envp[]) {
	int buttonValue = 0;

	// Create mutexes
	initOSDataStructs();
	initGlobals();
	initCreateTasks();

	// Clear button edge capture register and enable interrupts
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PUSH_BUTTON_BASE, 0x7);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PUSH_BUTTON_BASE, 0x7);

	// Register the IRQs
	alt_irq_register(FREQUENCY_ANALYSER_IRQ, 0, FreqAnalyserISR);
	alt_irq_register(PUSH_BUTTON_IRQ, 0, ButtonISR);
	alt_irq_register(PS2_IRQ, (void*)&buttonValue, KeyISR);

	vTaskStartScheduler();

	for (;;)
		;

	return 0;
}
