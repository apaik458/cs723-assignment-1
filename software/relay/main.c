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

void FreqAnalyserISR(void* context, alt_u32 id)
{
	;
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

	initOSDataStructs();
	initCreateTasks();
	vTaskStartScheduler();

	// Register the IRQs
  	alt_irq_register(ONCHIP_MEMORY_IRQ, (void*)&buttonValue, FreqAnalyserISR);
	alt_irq_register(PUSH_BUTTON_IRQ, (void*)&buttonValue, ButtonISR);
	alt_irq_register(PS2_IRQ, (void*)&buttonValue, KeyISR);

	for (;;);

	return 0;
}
