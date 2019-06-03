#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>

#include <graphics.h>
#include <macros.h>
#include "lcd.h"
#include "lcd_model.h"
#include "cpu_speed.h"
#include "graphics.h"
#include "macros.h"
#include "usb_serial.h"
#include "cab202_adc.h"

#include "starfighter.h"
#include "falling_objects.h"
#include "status.h"
#include "collision.h"


bool start;
bool paused;
bool game_over;

char key;


void introduction();
void setup();
void setup_game();
void start_game();
void run_game();
void draw_all();
void toggle_pause();
void read_keys();

#define TOGGLE_PAUSE BIT_IS_SET(PINB,0)
#define DISPLAY_STATUS BIT_IS_SET(PINB,7)
#define NEW_GAME BIT_IS_SET(PINF,6)
#define EXIT_GAME BIT_IS_SET(PINF,5)

int main(){

	setup();
	
	for (;;){

		read_keys();

		if (game_over) {
			clear_screen();
			draw_string(3,3,"GAME OVER",FG_COLOUR);
			show_screen();
			lcd_op_code_t op_code = lcd_set_display_mode;
			lcd_display_mode_t args = lcd_display_inverse;
			if (NEW_GAME) { 
				args = lcd_display_normal;
				start_game();
			}
			LCD_CMD(op_code, args);
		}
		else {
			// back light
			TC4H = 20 >> 8;
			OCR4A = 20 & 0xff;

			if (NEW_GAME) { 
				_delay_ms(150);
				start_game();
			}

			// pause / unpause game
			if ( TOGGLE_PAUSE ) {
				_delay_ms(150);
				toggle_pause();
			}

			if (start){
				if (paused) { pause_game(); }
				else {
					unpause_game();
					run_game();
				}
				draw_all();
				if (EXIT_GAME || no_lives()) { game_over = true; }
			}
			else { introduction(); }

			if ( DISPLAY_STATUS ) {
				if (paused) { enable_show_status(); }
				send_status();
			}
		}
	}
}

// setup system; lcd, ports, pins, timer,
void setup(){

	set_clock_speed(CPU_8MHz);

	usb_init();
	adc_init();
	lcd_init(LCD_DEFAULT_CONTRAST);

	TC4H = 1023 >> 8;
	OCR4C = 1023 & 0xff;
	TCCR4A = (1<<COM4A1) | (1<<PWM4A);
	SET_BIT(DDRC, 7);
	TCCR4B = (1<<CS42) | (1<<CS41) | (1<<CS40);
	TCCR4D = 0;

	CLEAR_BIT(DDRF, 6); // left tactile
	CLEAR_BIT(DDRF, 5); // right tactile
	CLEAR_BIT(DDRD, 1); // up joy
	CLEAR_BIT(DDRB, 1); // left joy
	CLEAR_BIT(DDRB, 7); // down joy
	CLEAR_BIT(DDRD, 0); // right joy
	CLEAR_BIT(DDRB, 0); // centre joy
	
	start = false;
	setup_game();
}

// setup the game, assign default values
void setup_game(){
	game_over = false;
	paused = true;
	setup_status();
	setup_starfighter();
	setup_objects();
}

// restart the game
void start_game(){
	setup_game();
	start = true;
	char* game_started = "\nNEW GAME STARTED\n\n\r";
	usb_serial_write((uint8_t*)game_started, strlen(game_started));
	send_status();
}

// run game -> main events (movements, collisions)
void run_game(){
	update_starfighter();
	update_objects();
	do_collisions();
}

// draw_everything
void draw_all(){
	clear_screen();
	draw_starfighter();
	draw_bolts();
	draw_objects();
	display_status();

	show_screen();

}

// introduction screen
void introduction(){

	clear_screen();

	// details
	draw_string(0, 0, "n10227555", FG_COLOUR);
	draw_string(0, 10, "Asteroid", FG_COLOUR);
	draw_string(0, 20, "Apocalypse", FG_COLOUR);

	
	// animation
	move_starfighter();
	if (starfighter_position() == STARFIGHTER_WIDTH/2){
		change_direction(RIGHT);
	}
	else if (starfighter_direction() == STATION) { change_direction(LEFT); }
	draw_starfighter();

	show_screen();

}

// pause or unpause game
void toggle_pause(){
	if (paused) {  paused = false; }
	else { paused = true; }
}


// read upto 10 digits from USB serial
int read_numbers(){
	char chars[10];
	int i = 0;
	while (1){
		if (usb_serial_available()){
			key = usb_serial_getchar();
			if (key == 13) { break; }
			else { 
				usb_serial_putchar(key);
				chars[i] = key; 
				i++; 
			}
			if (i == 10) { break; }
		}
	}
	usb_serial_write((uint8_t*)"\r\n\n",3);
	int amount;
	sscanf(chars, "%d", &amount);
	return amount;
}

void read_keys(){


	key = usb_serial_getchar();

	if (key == 'r') { start_game(); }
	if (key == 'q') { game_over = true; }

	if (key == 'l') {

		char* line = "Set amount of lives: ";
		usb_serial_write((uint8_t*)line, strlen(line));

		int lives = read_numbers();
		set_lives(lives);
	}

	if (key == 'g') {
		char* line = "Set the score: ";
		usb_serial_write((uint8_t*)line, strlen(line));

		int score = read_numbers();
		set_score(score);
	}

	if (key == 'h') {
		char* line = "Set the starfighter position (6-78) inclusive: ";
		usb_serial_write((uint8_t*)line, strlen(line));

		int new_pos = read_numbers();
		set_starfighter_position(new_pos);
	}

	if (paused == false){
		if (key=='a'){ change_direction(LEFT); }
		else if (key=='d'){ change_direction(RIGHT); }
		else if (key=='w'){ shoot_bolt(); }	
	}
	
	if (start){
		if (key == 'p'){
			toggle_pause();
		}
		else if (key=='s'){
			if (paused) { enable_show_status(); }
			send_status();
		}
	}

	usb_serial_flush_input();
	usb_serial_flush_output();




}


