/*
 * switchPollingTask.h
 *
 *  Created on: 13/04/2024
 *      Author: mtay527
 */

#ifndef SWITCHPOLLINGTASK_H_
#define SWITCHPOLLINGTASK_H_

#define LOAD_SWITCH_MASK ((0b1 << 5) - 1)

void switchPollingTask(void *pvParameters);

#endif /* SWITCHPOLLINGTASK_H_ */
