/*
 * evaluate.c
 *
 *  Created on: 18.09.2014
 *      Author: arno
 */

#include "evaluate.h"
#include "measurement.h"

uint32_t EVAL_get_average (volatile uint32_t *buff, uint8_t ch, uint32_t start, uint32_t end) {

uint16_t	*data;
ulong	result;
uint32_t	i;

	if (start == end)
		return 0;

	result = 0;
	buff += start;

	for (i = start; i < end; i++) {
		data = (uint16_t*)buff;
		data += ch;
		result += *data & 0xffff;
		buff++;
	}

	return result / (end - start);
}

void EVAL_calc_min_max_avg (volatile uint32_t *buff, uint8_t ch, uint32_t start, uint32_t end,
		uint16_t* min, uint16_t* max, uint16_t* avg) {

	uint16_t	*data;
	uint32_t	result;
	uint32_t	i;
	uint16_t	val;

		if (end >= MEASUREMENT_BLOCK_LENGTH)
			end = MEASUREMENT_BLOCK_LENGTH -1;
		if (start > end)
			start = end;

		*min = 0xffff;
		*max = 0;
		result = 0;
		buff += start;

		for (i = start; i <= end; i++) {
			data = (uint16_t*)buff;
			data += ch;
			val = *data & 0xffff;
			result += val;
			if (*min > val) *min = val;
			if (*max < val) *max = val;
			buff++;
		}

		*avg = result / (end - start +1);

}

/*
 *
 */
uint8_t EVAL_search_periode (volatile uint32_t *buff, uint8_t ch, uint32_t start, uint32_t end,
		uint16_t* start_t, uint16_t* end_t, uint16_t avg) {

	uint16_t	i;
	uint16_t	y1, y2;

		if (end >= MEASUREMENT_BLOCK_LENGTH)
			end = MEASUREMENT_BLOCK_LENGTH -1;
		if (start > end)
			start = end;

		buff += start;
		y1 = *((uint16_t*)buff + ch);
		i = start;

		while (i <= end) {

			y2 = *((uint16_t*)buff + ch);
			if ((y1 > avg) && (y2 <= avg)) {
				*start_t = i;
				break;
			}
			y1 = y2;
			buff++;
			i++;
		}

		while (i <= end) {

			y2 = *((uint16_t*)buff + ch);
			if ((y1 < avg) && (y2 >= avg)) {
				*end_t = i;
				break;
			}
			y1 = y2;
			buff++;
			i++;
		}

		if (i == end)
			return 0;

		return 1;
}


uint16_t	val_min[MEASUREMENT_CHANNELS];
uint16_t	val_max[MEASUREMENT_CHANNELS];
uint16_t	val_avg[MEASUREMENT_CHANNELS];
uint16_t	periode_start[MEASUREMENT_CHANNELS];
uint16_t	periode_end[MEASUREMENT_CHANNELS];
uint32_t 	*val_buff;

void EVAL_analyze_values (uint8_t b) {

int c;
uint16_t	t;

	val_buff = get_buffer_pointer (b);
	/* 1. step: calculate min, max and average of all channels */
	for (c = 0; c < MEASUREMENT_CHANNELS; c++) {
		EVAL_calc_min_max_avg (val_buff, c, 0, MEASUREMENT_BLOCK_LENGTH-1,
				&val_min[c], &val_max[c], &val_avg[c]);
	}

	/* 2. step: search one period of the wave */
	for (c = 0; c < MEASUREMENT_CHANNELS; c++) {

		EVAL_search_periode (val_buff, c, 0, MEASUREMENT_BLOCK_LENGTH-1,
				&periode_start[c], &periode_end[c], val_avg[c]);

	}
}

/* Note: EVAL_analyze values must have been called before */
uint16_t EVAL_get_min (uint8_t ch) {
	return val_min[ch];
}

/* Note: EVAL_analyze values must have been called before */
uint16_t EVAL_get_max (uint8_t ch) {
	return val_max[ch];
}

/* Note: EVAL_analyze values must have been called before */
uint16_t EVAL_get_avg (uint8_t ch) {
	return val_avg[ch];
}

/* Note: EVAL_analyze values must have been called before */
uint16_t EVAL_get_start (uint8_t ch) {
	return periode_start[ch];
}

/* Note: EVAL_analyze values must have been called before */
uint16_t EVAL_get_end (uint8_t ch) {
	return periode_end[ch];
}

/* Note: EVAL_analyze values must have been called before */
uint16_t EVAL_get_rpm (uint8_t ch) {
uint32_t	v;
	if ((val_max[ch] - val_min[ch]) < 100)
		return 0;
	v = periode_end[ch] - periode_start[ch];
	if (!v)
		return 0;
	v = 1200000 / v;
	return v;
}


uint32_t* EVAL_get_buff_p (void) {
	return val_buff;
}
