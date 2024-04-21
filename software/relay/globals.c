/*
 * globals.c
 *
 *  Created on: 17/04/2024
 *      Author: mtay527
 */

#include "globals.h"

void initGlobals() {
	isMaintenanceState = 0;
	xisStable = 1; // Default to stable state
    xfirstTick = -1; // Default to imposible state
}
