#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <graphics.h>
#include "lcd_model.h"
#include "starfighter.h"
#include "cab202_adc.h"
#include "macros.h"
#include "status.h"

#define POT_MAX 1023
#define PI 3.1415
#define STARFIGHTER_SPEED 0.5
#define BOLT_SPEED 0.5

const char* starfighter_bitmap=
"..      .."
". .    . ."
".  .  .  ."
".   ..   ."
"..........";
static int lives;
static int score;
static double position;
static double cannon_direction;



static double bolts_x[BOLTS_AMOUNT];
static double bolts_y[BOLTS_AMOUNT];
static double bolts_x_speed[BOLTS_AMOUNT];
static double bolts_y_speed[BOLTS_AMOUNT];
static int next_bolt;
static double last_fired;
static int bolts_amount;

dir direction;

void update_cannon_direction();
void setup_starfighter(){
	lives = 5;
	score = 0;
	position = LCD_X/2;
	update_cannon_direction();
	direction = STATION;

	for (int i = 0; i<BOLTS_AMOUNT; i++){
		bolts_x[i] = -1;
		bolts_y[i] = -1;
		bolts_x_speed[i] = 0;
		bolts_y_speed[i] = 0;
	}
	next_bolt = 0;
	last_fired = 0;
	bolts_amount = 0;
}

void handle_controls();
void update_cannon_direction();
void update_bolts();
void update_starfighter(){
	handle_controls();
	update_bolts();
	move_starfighter();
}

void shoot_bolt();
void handle_controls(){

	if (BIT_IS_SET(PINB, 1)) {
		if (direction == RIGHT) { direction = STATION; }
		else { direction = LEFT; }
	}
	else if (BIT_IS_SET(PIND, 0)) {
		if (direction == LEFT) { direction = STATION; }
		else { direction = RIGHT; }
	}
	else if (BIT_IS_SET(PIND, 1)){
		shoot_bolt();
	}
}

void update_cannon_direction(){
	uint16_t pot_1_value = adc_read(0);
	double pot_1_ratio = pot_1_value / (double) POT_MAX;
	int degrees = (int)(120*pot_1_ratio) + 30; // between 30 and 150 degrees
	cannon_direction = degrees;
}

void update_bolts(){

	next_bolt = BOLTS_AMOUNT;
	bolts_amount = 0;

	for (int i = 0; i < BOLTS_AMOUNT; i++){
		bolts_x[i] += bolts_x_speed[i];
		bolts_y[i] += bolts_y_speed[i];

		if (bolts_x[i] < 0 || bolts_x[i] > LCD_X || bolts_y[i] < 0){
			bolts_x[i] = -1;
			bolts_y[i] = -1;
			bolts_x_speed[i] = 0;
			bolts_y_speed[i] = 0;
			next_bolt = i;
		} else { bolts_amount += 1; }
	}
}

void move_starfighter(){

	if (direction != STATION){
		if (direction == LEFT && position > STARFIGHTER_WIDTH/2){
			position -= STARFIGHTER_SPEED;
		}
		else if (direction == RIGHT && position < LCD_X-STARFIGHTER_WIDTH/2){
			position += STARFIGHTER_SPEED;
		}
		else {
			direction = STATION;
		}
	}
}

void shoot_bolt(){

	double current_time = get_game_time();

	if (current_time - last_fired > 200 && next_bolt != BOLTS_AMOUNT){
		last_fired = current_time;
		bolts_x[next_bolt] = position;
		bolts_y[next_bolt] = LCD_Y-STARFIGHTER_HEIGHT/2;
		double radians = cannon_direction * PI/180;
		bolts_x_speed[next_bolt] = -cos(radians)*BOLT_SPEED;
		bolts_y_speed[next_bolt] = -sin(radians)*BOLT_SPEED;
	}

}

void draw_cannon();
void draw_shield();
void draw_starfighter(){

	int left = position-STARFIGHTER_WIDTH/2;
	int top = LCD_Y - STARFIGHTER_HEIGHT;

	for (int j = 0; j < STARFIGHTER_HEIGHT; j++){
        for (int i = 0; i < STARFIGHTER_WIDTH; i++){
            // get current pixel of bitmap

            char pixel = starfighter_bitmap[i + j*STARFIGHTER_WIDTH];

            if (pixel == '.'){
            	draw_pixel(left+i, top+j, FG_COLOUR);
            }
        }
    }
    draw_cannon();
    draw_shield();
}

void draw_cannon(){

	update_cannon_direction();

	int length = 7;

	double radians = cannon_direction * PI/180;

	int x = - (int)(cos(radians) * length);

	draw_line(position,LCD_Y-STARFIGHTER_HEIGHT/2,position+x,LCD_Y-length,FG_COLOUR);
	draw_line(position-1,LCD_Y-STARFIGHTER_HEIGHT/2,position+x-1,LCD_Y-length,FG_COLOUR);
}

void draw_shield(){

	for (int i = 0; i < LCD_X; i++){
		if (i % 2){
			draw_pixel(i, 39, FG_COLOUR);
		}
	}
}

void draw_bolts(){

	for (int i = 0; i<BOLTS_AMOUNT; i++){
		draw_pixel(bolts_x[i],bolts_y[i],FG_COLOUR);
		draw_pixel(bolts_x[i]-1,bolts_y[i],FG_COLOUR);
		draw_pixel(bolts_x[i],bolts_y[i]-1,FG_COLOUR);
		draw_pixel(bolts_x[i]-1,bolts_y[i]-1,FG_COLOUR);
	}
}



void change_direction(dir new_direction){
	direction = new_direction;
}

void set_starfighter_position(int new_pos){
	if (new_pos-STARFIGHTER_WIDTH/2>0 && new_pos+STARFIGHTER_WIDTH/2<LCD_X){
		position = new_pos;
	}
}
int starfighter_position(){
	return position;
}

dir starfighter_direction(){
	return direction;
}


int get_bolt_x(int i) { return bolts_x[i]; }
int get_bolt_y(int i) { return bolts_y[i]; }

void clear_bolt(int i) {
	bolts_x[i] = -1;
	bolts_y[i] = -1;
	bolts_x_speed[i] = 0;
	bolts_y_speed[i] = 0;
	next_bolt = i;
}



void decrease_lives(int amount) { lives -= amount; }
void increase_score(int amount) { score += amount; }

int get_lives() { return lives; }
int get_score() { return score; }

void set_lives(int amount) { lives = amount; }
void set_score(int amount) { score = amount; }

bool no_lives() {
	if (lives <= 0) { return true; }
	return false;
}

int get_bolts_amount() { return bolts_amount; }

int get_cannon_direction(){ return cannon_direction; }