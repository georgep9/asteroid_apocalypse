#include <avr/io.h> 
#include <macros.h>

#include "read_controls.h"

// State machine for "button pressed"
bool pressed = false;
uint16_t closed_num = 0;
uint16_t open_num = 0;

#define THRESHOLD (1000)

bool is_pressed(button button_pressed) {
	// Detect a Click on left button
	bool was_pressed = pressed;

	bool bit_is_set = true;

	if (button_pressed == j_up) { bit_is_set = BIT_IS_SET(PIND, 1); }
	else if (button_pressed == j_left) { bit_is_set = BIT_IS_SET(PINB, 1); }
	else if (button_pressed == j_down) { bit_is_set = BIT_IS_SET(PINB, 7); }
	else if (button_pressed == j_right) { bit_is_set = BIT_IS_SET(PIND, 0); }
	else if (button_pressed == left) { bit_is_set = BIT_IS_SET(PINF, 6); }

	if ( bit_is_set ) {
		closed_num++;
		open_num = 0;

		if ( closed_num  > THRESHOLD ) {
			if ( !pressed ) {
				closed_num = 0;
			}

			pressed = true;
		}
	}
	else {
		open_num++;
		closed_num = 0;

		if ( open_num > THRESHOLD ) {
			if ( pressed ) {
				open_num = 0;
			}

			pressed = false;
		}
	}

	return was_pressed && !pressed;
}