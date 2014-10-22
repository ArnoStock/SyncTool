/*
 * gfxobjects.h
 *
 *  Created on: 21.09.2014
 *      Author: arno
 */

#ifndef GFXOBJECTS_H_
#define GFXOBJECTS_H_

#define	COLOR_TYPE	uint16_t

typedef enum touch_type touch_type;
enum touch_type {released, down, pressed};

typedef struct gfx_touch_event_type gfx_touch_event_type;
struct gfx_touch_event_type {
	uint16_t	x;
	uint16_t	y;
	touch_type	t;
};

typedef struct gfx_element_location_type gfx_element_location_type;
struct gfx_element_location_type{
	/* location of the top-left corner */
	uint16_t	x;
	uint16_t	y;
};

typedef struct gfx_element_size_type gfx_element_size_type;
struct gfx_element_size_type {
	/* dimension of the element */
	uint16_t	w;
	uint16_t	h;
};

typedef struct gfx_element_control_type gfx_element_control_type;
struct gfx_element_control_type {
	/* function to draw element on page */
	void (*draw)(void*);
	/* function to check element touch event */
	void (*check)(void*, void*);
};


/* definition of gfx elements */

typedef struct gfx_common_descriptor_type gfx_common_descriptor_type;
struct gfx_common_descriptor_type {
	gfx_common_descriptor_type *next;
	gfx_element_control_type	ctrl;
	gfx_element_location_type	loc;
};

typedef struct gfx_button_descriptor_type gfx_button_descriptor_type;
struct gfx_button_descriptor_type {
	gfx_common_descriptor_type	desc;
	gfx_element_size_type		size;

	char*					text;
	COLOR_TYPE				face_col;
	COLOR_TYPE				border_col;
	COLOR_TYPE				text_col;
	COLOR_TYPE				down_col;
} ;

typedef struct gfx_label_descriptor_type gfx_label_descriptor_type;
struct gfx_label_descriptor_type {
	gfx_common_descriptor_type	desc;

	char*					text;
	COLOR_TYPE				text_col;
	COLOR_TYPE				back_col;
};

/* page descriptor */

typedef struct gfx_page_descriptor_type gfx_page_descriptor_type;
struct gfx_page_descriptor_type {
	COLOR_TYPE					col;
	gfx_common_descriptor_type	*elements;
};


/* switch to a different page */
void gfx_set_page (int8_t);
uint8_t gfx_get_active_page (void);
/* process touch event */
void gfx_process_touch_event (gfx_touch_event_type*);

/* functions for element type "label" */
void gfx_draw_label (void*);

/* functions for element type "button" */
void gfx_draw_button (void*);
void gfx_button_event (void*, void*);
void gfx_button_event_next_page (void*, void*);
void gfx_button_event_prev_page (void*, void*);

/* void functions for drawing and event processing */
void gfx_draw_nothing (void*);
void gfx_ignore_event (void*, void*);

/* poll the LCD touch controller */
void gfx_poll_touch_controller (void);

/* draw graph from buffer data */
void gfx_draw_graph (uint16_t, uint16_t, uint32_t*, uint8_t, uint16_t, float, float, uint16_t, uint16_t);


#endif /* GFXOBJECTS_H_ */
