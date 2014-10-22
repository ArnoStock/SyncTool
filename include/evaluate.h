/*
 * evaluate.h
 *
 *  Created on: 18.09.2014
 *      Author: arno
 */

#include "SyncToolA.h"

#ifndef EVALUATE_H_
#define EVALUATE_H_

uint32_t EVAL_get_average (volatile uint32_t*, uint8_t, uint32_t, uint32_t);
void EVAL_calc_min_max_avg (volatile uint32_t*, uint8_t, uint32_t, uint32_t, uint16_t*, uint16_t*, uint16_t*);

void EVAL_analyze_values (uint8_t b);

uint16_t EVAL_get_min (uint8_t);
uint16_t EVAL_get_max (uint8_t);
uint16_t EVAL_get_avg (uint8_t);
uint16_t EVAL_get_rpm (uint8_t);

#endif // EVALUATE_H_
