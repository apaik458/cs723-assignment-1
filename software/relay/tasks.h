/*
 * tasks.h
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#ifndef TASKS_H_
#define TASKS_H_

// Scheduler includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Source includes
#include "tasks/defines.h"
#include "tasks/stabilityMonitorTask.h"
#include "tasks/shouldShedTask.h"
#include "tasks/shouldReconnectTask.h"
#include "tasks/loadCtrlTask.h"
#include "tasks/switchPollingTask.h"
#include "tasks/ledTask.h"
#include "tasks/vgaTask.h"

void initCreateTasks();

#endif /* TASKS_H_ */
