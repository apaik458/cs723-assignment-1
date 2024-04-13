/*
 * ledTask.h
 *
 *  Created on: 13/04/2024
 *      Author: sale188
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include "defines.h"

void FreqAnalyserISR(void* context, alt_u32 id);
void ButtonISR(void* context, alt_u32 id);
void KeyISR(void* context, alt_u32 id);
#endif