/*
 * gfxobjects.c
 *
 *  Created on: 21.09.2014
 *      Author: arno
 */

#include "SyncToolA.h"

extern const gfx_page_descriptor_type pages[];

int8_t current_page;

void gfx_set_page (int8_t p) {

gfx_common_descriptor_type	*g;

	if (p < 0) p = get_page_count() -1;
	if (p >= get_page_count()) p = 0;

	current_page = p;
	LCD_Clear(pages[p].col);
//	LCD_FillRect (0, 0, MAX_X-1, MAX_Y-1, pages[p].col);
	g = pages[p].elements;
	while (g) {
		g->ctrl.draw(g);
		g = g->next;
	}
}

uint8_t gfx_get_active_page () {
	return current_page;
}

gfx_touch_event_type touch_event;
void gfx_page_process_touch_event (Coordinate* c, touch_type t) {

gfx_common_descriptor_type	*g;

	touch_event.x = c->x;
	touch_event.y = c->y;
	touch_event.t = t;
	g = pages[current_page].elements;
	while (g) {
		g->ctrl.check(g, &touch_event);
		g = g->next;
	}

}


/***************************************/
/* common functions for all elements   */
/***************************************/
uint8_t gfx_is_hit (gfx_element_location_type* r,
					gfx_element_size_type* s,
					gfx_touch_event_type* t) {
	if (t->x < r->x) return 0;
	if (t->y < r->y) return 0;
	if (t->x > r->x + s->w) return 0;
	if (t->y > r->y + s->h) return 0;
	return 1;
}


/***************************************/
/* functions for element type "button" */
/***************************************/
void draw_button_up (gfx_button_descriptor_type *b) {
	LCD_FillRect (b->desc.loc.x, b->desc.loc.y,
				b->desc.loc.x + b->size.w, b->desc.loc.y  + b->size.h,
				b->border_col);
	LCD_FillRect (b->desc.loc.x +1, b->desc.loc.y +1,
				b->desc.loc.x + b->size.w -2, b->desc.loc.y  + b->size.h -2,
				b->face_col);
	GUI_Text (b->desc.loc.x +4, b->desc.loc.y +3, b->text, b->text_col, b->face_col);
}

void draw_button_down (gfx_button_descriptor_type *b) {
	LCD_FillRect (b->desc.loc.x, b->desc.loc.y,
				b->desc.loc.x + b->size.w, b->desc.loc.y  + b->size.h,
				b->border_col);
	LCD_FillRect (b->desc.loc.x +2, b->desc.loc.y +2,
				b->desc.loc.x + b->size.w -4, b->desc.loc.y  + b->size.h -4,
				b->face_col);
	GUI_Text (b->desc.loc.x +5, b->desc.loc.y +4, b->text, b->text_col, b->face_col);
}

void gfx_draw_button (void* d) {
	draw_button_up (d);
}

void gfx_button_event_next_page (void* d, void* e) {
	gfx_button_descriptor_type* b = d;
	gfx_touch_event_type* t = e;
	if (t->t == released) {
		draw_button_up (d);
		if (gfx_is_hit (&(b->desc.loc), &(b->size), t))
			gfx_set_page (current_page+1);
	}
	else if ((t->t == pressed) && (gfx_is_hit (&(b->desc.loc), &(b->size), t))) {
		draw_button_down (d);
	}
}

void gfx_button_event_prev_page (void* d, void* e) {
	gfx_button_descriptor_type *b = d;
	gfx_touch_event_type* t = e;
	if (t->t == released) {
		draw_button_up (d);
		if (gfx_is_hit (&(b->desc.loc), &(b->size), t))
			gfx_set_page (current_page-1);
	}
	else if ((t->t == pressed) && (gfx_is_hit (&(b->desc.loc), &(b->size), t))) {
		draw_button_down (d);
	}
}


/***************************************/
/* functions for element type "button" */
/***************************************/
void gfx_draw_label (void* e) {

	gfx_label_descriptor_type *l = e;
	GUI_Text(l->desc.loc.x, l->desc.loc.y, l->text,l->text_col, l->back_col);

}

/* void functions for drawing and event processing */
void gfx_draw_nothing (void* d) {

}
void gfx_ignore_event (void *d, void *e) {

}

#define MAX_TOUCH_ACTIVE	6
volatile uint8_t touch_active;
Coordinate screen, last_screen;

/* poll the LCD touch controller */
void gfx_poll_touch_controller (void) {

	if (Read_TouchPosition(&screen)) {
		if (!touch_active) {
			/* fire event: the touch has been pressed */
			gfx_page_process_touch_event (&screen, pressed);
		}
		else {
			/* fire event: the touch is still pressed */
			gfx_page_process_touch_event (&screen, down);
		}
		touch_active = MAX_TOUCH_ACTIVE;
		last_screen = screen;
	}
	else if (touch_active) {
		touch_active--;
		if (!touch_active) {
			/* fire event: the touch has been released */
			gfx_page_process_touch_event (&last_screen, released);
		}
	}
}

void gfx_draw_graph (uint16_t start_x, uint16_t start_y, uint32_t *buff, uint8_t ch,
							uint16_t count, float scale_x, float scale_y, uint16_t ofs, uint16_t col) {

int i;
float v;
float x;

	for (i = 0; i < count; i++) {

		v = ch ? (*buff >> 16) & 0xffff : *buff & 0xffff;
		v -= ofs;
		v *= scale_y;
		v = start_y - v;

		x = start_x + i;
		x *= scale_x;

		LCD_SetPoint (x, v, col);

		buff += (MEASUREMENT_CHANNELS / 2);
	}



}

