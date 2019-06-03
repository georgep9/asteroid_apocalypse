#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <macros.h>
#include <graphics.h>
#include "status.h"
#include "starfighter.h"
#include "usb_serial.h"
#include "cab202_adc.h"
#include "falling_objects.h"

static bool paused = false;
static bool show_status = false;

static double game_time;
static int minutes;
static int seconds;

volatile double overflow_counter = 0;

void setup_status(){

	
	game_time = 0;
	minutes = 0;
	seconds = 0;
	overflow_counter = 0;

	CLEAR_BIT(TCCR0B,WGM02);
	SET_BIT(TCCR0B,CS02);
	CLEAR_BIT(TCCR0B,CS01);
	SET_BIT(TCCR0B,CS00);
	SET_BIT(TIMSK0, TOIE0);
	sei();

}

void display_status(){
	if (paused && show_status){
		clear_screen();
		char time[16];
		snprintf(time, sizeof(time), "Time: %02d:%02d", minutes, seconds);
		draw_string(0, 0, time, FG_COLOUR);

		char lives[16];
		snprintf(lives, sizeof(lives), "Lives: %d", get_lives());
		draw_string(0, 10, lives, FG_COLOUR);

		char score[16];
		snprintf(score, sizeof(score), "Score: %d", get_score());
		draw_string(0,20,score, FG_COLOUR);
	}
}

ISR(TIMER0_OVF_vect) {
	overflow_counter ++;
	if (paused == false) {
		game_time += 33.33;
		seconds = (int)(game_time / 1000) % 60;
		minutes = (int)(game_time / 1000 / 60);
	}
	if (overflow_counter > 30){
		overflow_counter = 0;
	}
}

double get_game_time() { return game_time; }

double get_time_count() { return overflow_counter; }

void pause_game() { paused = true; }
void unpause_game() { 
	paused = false;
	show_status = false;
}

void enable_show_status() {
	show_status = true;
}	

void send_status(){

	char time[32];
	snprintf(time, sizeof(time), "Time: %02d:%02d\n\r", minutes, seconds);
	usb_serial_write((uint8_t*)time, strlen(time)); // transmit a buffer

	char lives[32];
	snprintf(lives, sizeof(lives), "Lives: %d\n\r", get_lives());
	usb_serial_write((uint8_t*)lives, strlen(lives)); // transmit a buffer

	char score[32];
	snprintf(score, sizeof(score), "Score: %d\n\r", get_score());
	usb_serial_write((uint8_t*)score, strlen(score)); // transmit a buffer

	char asteroids[32];
	snprintf(asteroids, sizeof(asteroids), "Asteroids: %d\n\r", get_asteroids_amount());
	usb_serial_write((uint8_t*)asteroids, strlen(asteroids)); // transmit a buffer

	char boulders[32];
	snprintf(boulders, sizeof(boulders), "Boulders: %d\n\r", get_boulders_amount());
	usb_serial_write((uint8_t*)boulders, strlen(boulders)); // transmit a buffer

	char fragments[32];
	snprintf(fragments, sizeof(fragments), "Fragments: %d\n\r", get_fragments_amount());
	usb_serial_write((uint8_t*)fragments, strlen(fragments)); // transmit a buffer

	char bolts[32];
	snprintf(bolts, sizeof(bolts), "Plasma Bolts: %d\n\r", get_bolts_amount());
	usb_serial_write((uint8_t*)bolts, strlen(bolts)); // transmit a buffer

	char aim[32];
	snprintf(aim,sizeof(aim), "Aim: %d\n\r",get_cannon_direction()-90);
	usb_serial_write((uint8_t*)aim, strlen(aim));

	char speed[32];
	snprintf(speed, sizeof(speed), "Speed: %d\n\n\r", adc_read(1));
	usb_serial_write((uint8_t*)speed, strlen(speed));

}
