/*
 * measurement.h
 *
 *  Created on: 14.09.2014
 *      Author: Arno Stock
 */

#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

#include "SyncToolA.h"

#define MEASUREMENT_BLOCK_LENGTH	2048
#define MEASUREMENT_CHANNELS		2
#define	MEASUREMENT_MAX_VALUE		4095
#define MEASUREMENT_MIN_VALUE		0

#define		MEASUREMENT_BUFFER_FULL_FLAG	0x80
extern uint8_t		evaluate_buffer;

void init_measurement_subsystem (void);
uint16_t get_buffer_value (uint8_t, uint8_t, uint16_t);
uint16_t get_buffer_average (uint8_t, uint8_t, uint16_t, uint16_t);
volatile void* get_buffer_pointer (uint8_t);


#endif /* MEASUREMENT_H_ */
