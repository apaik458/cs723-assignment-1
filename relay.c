// Standard includes
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// Scheduler includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "system.h"
#include <altera_avalon_pio_regs.h>
#include "sys/alt_irq.h"

// Definition of Task Stacks
#define TASK_STACKSIZE 2048

// Definition of Task Priorities
#define STABILITYMONITORTASK_PRIORITY 1
#define SHOULDSHEDTASK_PRIORITY       1
#define SHOULDRECONNECTTASK_PRIORITY  1
#define LOADCTRLTASK_PRIORITY         1
#define SWITCHPOLLINGTASK_PRIORITY    1
#define LEDTASK_PRIORITY              1
#define VGATASK_PRIORITY              1

/* The parameters passed to the reg test tasks.  This is just done to check
 the parameter passing mechanism is working correctly. */
#define mainREG_TEST_1_PARAMETER    ((void *) 0x12345678)
#define NULL                        ((void *) 0x87654321)
#define mainREG_TEST_PRIORITY       (tskIDLE_PRIORITY + 1)

// Definition of Message Queue
#define MSG_QUEUE_SIZE 30
QueueHandle_t newFreqQ;
QueueHandle_t isStableQ;
QueueHandle_t shedQ;
QueueHandle_t reconnectQ;
QueueHandle_t freqMeasureQ;
QueueHandle_t keyPressQ;
QueueHandle_t switchStateQ;
QueueHandle_t maintenanceQ;
QueueHandle_t loadCtrlQ;

// used to delete a task
TaskHandle_t xHandle;

// Definition of Semaphore
SemaphoreHandle_t shared_resource_sem;

static void stabilityMonitorTask(void *pvParameters);
static void shouldShedTask(void *pvParameters);
static void shouldReconnectTask(void *pvParameters);
static void loadCtrlTask(void *pvParameters);
static void switchPollingTask(void *pvParameters);
static void LEDTask(void *pvParameters);
static void VGATask(void *pvParameters);

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

int main(int argc, char* argv[], char* envp[])
{
	initOSDataStructs();
	initCreateTasks();
	vTaskStartScheduler();

	// register the ISRs
  	alt_irq_register(ONCHIP_MEMORY_IRQ, (void*)&buttonValue, FreqAnalyserISR);
	alt_irq_register(PUSH_BUTTON_IRQ, (void*)&buttonValue, ButtonISR);
	alt_irq_register(PS2_IRQ, (void*)&buttonValue, KeyISR);

	for (;;);

	return 0;
}

// This function creates the message queues and semaphores
int initOSDataStructs(void)
{
	newFreqQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	freqMeasureQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	isStableQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	shedQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	reconnectQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	switchStateQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	keyPressQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	maintenanceQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	loadCtrlQ = xQueueCreate(MSG_QUEUE_SIZE, sizeof(void*));
	shared_resource_sem = xSemaphoreCreateCounting(9999, 1);
	return 0;
}

// This function creates the tasks used in this example
int initCreateTasks(void)
{
	/* The RegTest tasks as described at the top of this file. */
	xTaskCreate(stabilityMonitorTask, "stabilityMonitorTask", TASK_STACKSIZE, NULL, STABILITYMONITORTASK_PRIORITY, NULL);
	xTaskCreate(shouldShedTask,       "shouldShedTask",       TASK_STACKSIZE, NULL, SHOULDSHEDTASK_PRIORITY, NULL);
	xTaskCreate(shouldReconnectTask,  "shouldReconnectTask",  TASK_STACKSIZE, NULL, SHOULDRECONNECTTASK_PRIORITY, NULL);
	xTaskCreate(loadCtrlTask,         "loadCtrlTask",         TASK_STACKSIZE, NULL, LOADCTRLTASK_PRIORITY, NULL);
	xTaskCreate(switchPollingTask,    "switchPollingTask",    TASK_STACKSIZE, NULL, SWITCHPOLLINGTASK_PRIORITY, NULL);
	xTaskCreate(LEDTask,              "LEDTask",              TASK_STACKSIZE, NULL, LEDTASK_PRIORITY, NULL);
	xTaskCreate(VGATask,              "VGATask",              TASK_STACKSIZE, NULL, VGATASK_PRIORITY, NULL);
	return 0;
}

static void stabilityMonitorTask(void *pvParameters)
{
	;
}

static void shouldShedTask(void *pvParameters)
{
	;
}

static void shouldReconnectTask(void *pvParameters)
{
	;
}

static void loadCtrlTask(void *pvParameters)
{
	;
}

static void switchPollingTask(void *pvParameters)
{
	;
}

static void LEDTask(void *pvParameters)
{
	;
}

static void VGATask(void *pvParameters)
{
	;
}
