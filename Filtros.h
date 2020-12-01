/*
 * Filtros.h
 *
 *  Created on: 29 nov. 2020
 *      Author: urik_
 */

#ifndef FILTROS_H_
#define FILTROS_H_

#include "arm_math.h"

#define VAL_NUMBERS 50
#define BLOCK 1024
#define LENGTH_SAMPLES 4096

void FILTER_LP();
void FILTER_HP();
void FILTER_BP();

void Data_Buffer(uint32_t *Buffer);
void Call_Filter(uint8_t Type);

typedef enum
{
	LP,
	HP,
	BP
} filters;

#endif /* FILTROS_H_ */
