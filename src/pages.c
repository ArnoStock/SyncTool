/*
 * pages.c
 *
 *  Created on: 21.09.2014
 *      Author: arno
 */

#include "SyncToolA.h"
#include "stdio.h"

/* common definitions for all pages. Must be last in the lists */

const gfx_button_descriptor_type common_button2 =
	{ {(gfx_common_descriptor_type*)NULL, {&gfx_draw_button, &gfx_button_event_prev_page}, {5, 215}}, {42, 23},
			"Prev", White, Black, Black, Yellow};
const gfx_button_descriptor_type common_button1 =
	{ {(gfx_common_descriptor_type*)&common_button2, {&gfx_draw_button, &gfx_button_event_next_page}, {270, 215}}, {42, 23},
			"Next", White, Black, Black, Yellow};
const gfx_label_descriptor_type common_label1 =
	{ {(gfx_common_descriptor_type*)&common_button1, {&gfx_draw_label, &gfx_ignore_event}, {100, 1}}, "Sync Tool V0.1", Blue, White};

/* definition of page 0 */

const gfx_label_descriptor_type page0_label1 =
	{ {(gfx_common_descriptor_type*)&common_label1, {&gfx_draw_label, &gfx_ignore_event}, {68, 30}}, "Hello world!", Blue, White};


/* definition of page 1 */

const gfx_label_descriptor_type page1_label2 =
	{ {(gfx_common_descriptor_type*)&common_label1, {&gfx_draw_label, &gfx_ignore_event}, {1, 150}}, "Cyl 2", Blue, White};
const gfx_label_descriptor_type page1_label1 =
	{ {(gfx_common_descriptor_type*)&page1_label2, {&gfx_draw_label, &gfx_ignore_event}, {1, 90}}, "Cyl 1", Blue, White};

/* definition of page 2 */

const gfx_label_descriptor_type page2_label4 =
	{ {(gfx_common_descriptor_type*)&common_label1, {&gfx_draw_label, &gfx_ignore_event}, {5, 90}}, "Ch2:", Blue, White};
const gfx_label_descriptor_type page2_label3 =
	{ {(gfx_common_descriptor_type*)&page2_label4, {&gfx_draw_label, &gfx_ignore_event}, {5, 75}}, "Ch1: ", Blue, White};
const gfx_label_descriptor_type page2_label2 =
	{ {(gfx_common_descriptor_type*)&page2_label3, {&gfx_draw_label, &gfx_ignore_event}, {5, 60}}, "ADC reading", Blue, White};
const gfx_label_descriptor_type page2_label1 =
	{ {(gfx_common_descriptor_type*)&page2_label2, {&gfx_draw_label, &gfx_ignore_event}, {10, 30}}, "System Info", Blue, White};

/* definition of all pages */

const gfx_page_descriptor_type pages[] = { 	{White, (gfx_common_descriptor_type*)&page0_label1},
											{White, (gfx_common_descriptor_type*)&page1_label1},
											{White, (gfx_common_descriptor_type*)&page2_label1}};

int8_t get_page_count (void) {
	return sizeof (pages) / sizeof (gfx_page_descriptor_type);
}

#define BAR_MIN_X	0

#define GRAPH_START_X	3
#define GRAPH_LOWER_LIMIT	210
#define GRAPH_UPPER_LIMIT	30
uint16_t GRAPH_COLOR[] = { Red, Blue };


float calc_scale_y (uint8_t c) {

float scale_y;

	scale_y = GRAPH_LOWER_LIMIT - GRAPH_UPPER_LIMIT +1; /* display Y-range */
	scale_y /= (float)(EVAL_get_max (c) - EVAL_get_min (c)); /* range of measured values */
	return scale_y;
}

void update_active_page () {

char s[30];
uint32_t val;
float scale_y;
uint8_t i;
uint32_t *b;

	if (gfx_get_active_page() == 0) {

		LCD_FillRect ( GRAPH_START_X, GRAPH_UPPER_LIMIT-1, MAX_X-1, GRAPH_LOWER_LIMIT, White);

		scale_y = calc_scale_y (0);
		for (i = 1; i < MEASUREMENT_CHANNELS; i++) {
			if (scale_y > calc_scale_y (i))
				scale_y = calc_scale_y (i);
		}
		for (i = 0; i < MEASUREMENT_CHANNELS; i++) {
			b = EVAL_get_buff_p();
			if (EVAL_get_start(i) < MEASUREMENT_BLOCK_LENGTH -310)
				b += EVAL_get_start(i);
			gfx_draw_graph (GRAPH_START_X, GRAPH_LOWER_LIMIT, b, i, 310, 1.0, scale_y, EVAL_get_min (i), GRAPH_COLOR[i]);
		}
	}
	else if (gfx_get_active_page() == 1) {

		LCD_FillRect ( BAR_MIN_X, 115, MAX_X-1, 130, Grey);
		LCD_FillRect ( BAR_MIN_X, 175, MAX_X-1, 190, Grey);
		val = EVAL_get_avg (0) * (MAX_X - BAR_MIN_X) / MEASUREMENT_MAX_VALUE;
		LCD_FillRect ( BAR_MIN_X, 115, BAR_MIN_X + val, 130, Black);
		val = EVAL_get_avg (1) * (MAX_X - BAR_MIN_X) / MEASUREMENT_MAX_VALUE;
		LCD_FillRect ( BAR_MIN_X, 175, BAR_MIN_X + val, 190, Black);

	}
	else if (gfx_get_active_page() == 2) {

//		sprintf (s, "%4.4d", get_buffer_value(buffer & 0x0f, 0, 0));
		sprintf (s, "%4.4d / %4.4d / %4.4d %5.5drpm", EVAL_get_min (0), EVAL_get_avg (0), EVAL_get_max(0), EVAL_get_rpm(0));
		GUI_Text(40, 75, s, Red, White);
//		sprintf (s, "%4.4d", get_buffer_value(buffer & 0x0f, 1, 0));
		sprintf (s, "%4.4d / %4.4d / %4.4d %5.5drpm", EVAL_get_min (1), EVAL_get_avg (1), EVAL_get_max(1), EVAL_get_rpm(1));
		GUI_Text(40, 90, s, Black, White);

	}
}


