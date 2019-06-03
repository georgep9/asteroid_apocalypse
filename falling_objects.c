#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <graphics.h>
#include "cab202_adc.h"
#include "falling_objects.h"
#include "starfighter.h"

#define POT_MAX 1023


const char* asteroid_bitmap =
"   .   "
"  ...  "
" ..... "
"......."
" ..... "
"  ...  "
"   .   ";

static int asteroids_x[ASTEROID_AMOUNT];
static int asteroid_is_falling[ASTEROID_AMOUNT];
static int amount_asteroids;


const char* boulder_bitmap = 
"  .  "
" ... "
"....."
" ... "
"  .  ";
static int boulders_x[BOULDER_AMOUNT];
static int boulder_is_falling[BOULDER_AMOUNT];
static int amount_boulders;


const char* fragment_bitmap = 
" . "
"..."
" . ";
static int fragments_x[FRAGMENT_AMOUNT];
static int fragment_is_falling[FRAGMENT_AMOUNT];
static int amount_fragments;

static int amount_objects;

static double global_y;
static double falling_speed;


void setup_objects(){

	for (int i = 0; i<FRAGMENT_AMOUNT; i++){
		if (i < ASTEROID_AMOUNT) {
			int x = rand() % (LCD_X - ASTEROID_SIZE/2 + 1 - ASTEROID_SIZE/2) + ASTEROID_SIZE/2;
			asteroids_x[i] = x;
			asteroid_is_falling[i] = 1;
		}
		if (i < amount_boulders){
			boulders_x[i] = 0;
			boulder_is_falling[i] = 0;
		}
		fragments_x[i] = 0;
		fragment_is_falling[i] = 0;
	}

	global_y = -ASTEROID_SIZE/2-1;

	amount_objects = ASTEROID_AMOUNT;
	amount_asteroids = ASTEROID_AMOUNT;
	amount_boulders = 0;
	amount_fragments = 0;

	falling_speed = 0.25;
}

void move_objects();
void remove_objects();
void spawn_wave();
void update_objects(){

	uint16_t pot_1_value = adc_read(1);
	falling_speed = pot_1_value / (double) POT_MAX / 2;

	move_objects();
	if (global_y >= 38 - ASTEROID_SIZE/2 || amount_objects <= 0){ spawn_wave(); }
}

void draw_asteroids();
void draw_boulders();
void draw_fragments();
void draw_objects(){
	draw_asteroids();
	draw_boulders();
	draw_fragments();
}

void move_objects(){
	global_y += falling_speed;
}

void spawn_wave(){

	decrease_lives(amount_objects);

	for (int i = 0; i<FRAGMENT_AMOUNT; i++){
		if (i < ASTEROID_AMOUNT){			
			int x = rand() % (LCD_X - ASTEROID_SIZE/2 + 1 - ASTEROID_SIZE/2) + ASTEROID_SIZE/2;
			asteroids_x[i] = x;
			asteroid_is_falling[i] = 1;
			boulder_is_falling[i*2] = 0;
			boulder_is_falling[i*2+1] = 0;
		}
		fragments_x[i] = 0;
		fragment_is_falling[i] = 0;
	}

	global_y = -ASTEROID_SIZE/2-1;
	amount_objects = ASTEROID_AMOUNT;
	amount_asteroids = ASTEROID_AMOUNT;
	amount_boulders = 0;
	amount_fragments = 0;
}

void break_asteroid(int i) { 
	amount_asteroids -= 1;
	amount_objects -= 1;
	asteroid_is_falling[i] = 0;

	amount_boulders += 2;
	amount_objects += 2;

	boulders_x[i*2] = asteroids_x[i]-BOULDER_SIZE;
	boulders_x[i*2+1] = asteroids_x[i]+BOULDER_SIZE;
	if (boulders_x[i*2] < BOULDER_SIZE/2) { 
		boulders_x[i*2] = BOULDER_SIZE/2;
		boulders_x[i*2+1] = BOULDER_SIZE;
	}
	else if (boulders_x[i*2+1] > LCD_X - BOULDER_SIZE/2){
		boulders_x[i*2+1]  = LCD_X - BOULDER_SIZE/2;
		boulders_x[i*2] = LCD_X - BOULDER_SIZE;
	}

	boulder_is_falling[i*2] = 1;
	boulder_is_falling[i*2+1] = 1;
}

void break_boulder(int i){
	amount_boulders -= 1;
	amount_objects -= 1;
	boulder_is_falling[i] = 0;

	amount_fragments += 2;
	amount_objects += 2;

	fragments_x[i*2] = boulders_x[i]-FRAGMENT_SIZE;
	fragments_x[i*2+1] = boulders_x[i]+FRAGMENT_SIZE;
	if (fragments_x[i*2] < FRAGMENT_SIZE/2) { 
		fragments_x[i*2] = FRAGMENT_SIZE/2;
		fragments_x[i*2+1] = FRAGMENT_SIZE;
	}
	else if (fragments_x[i*2+1] > LCD_X - FRAGMENT_SIZE/2){
		fragments_x[i*2+1]  = LCD_X - FRAGMENT_SIZE/2;
		fragments_x[i*2] = LCD_X - FRAGMENT_SIZE;
	}

	fragment_is_falling[i*2] = 1;
	fragment_is_falling[i*2+1] = 1;
}

void break_fragment(int i){
	amount_fragments -= 1;
	amount_objects -= 1;
	fragment_is_falling[i] = 0;
}

void draw_asteroids(){
	for (int i = 0; i<ASTEROID_AMOUNT; i++){
		if (asteroid_is_falling[i]){
			int left = asteroids_x[i] - ASTEROID_SIZE/2;
			int top = global_y - ASTEROID_SIZE/2;
			for (int j = 0; j < ASTEROID_SIZE; j++){
		        for (int i = 0; i < ASTEROID_SIZE; i++){
		            char pixel = asteroid_bitmap[i + j*ASTEROID_SIZE];
		            if (pixel == '.'){
		            	draw_pixel(left+i, top+j, FG_COLOUR);
		            }
		        }
		    }
		}	
	}
}

void draw_boulders(){
	for (int i = 0; i<BOULDER_AMOUNT; i++){
		if (boulder_is_falling[i]){
			int left = boulders_x[i] - BOULDER_SIZE/2;
			int top = global_y - BOULDER_SIZE/2;
			for (int j = 0; j < BOULDER_SIZE; j++){
				for (int i = 0; i < BOULDER_SIZE; i++){
					char pixel = boulder_bitmap[i+j*BOULDER_SIZE];
					if (pixel == '.'){
						draw_pixel(left+i, top+j, FG_COLOUR);
					}
				}
			}
		}
	}
}

void draw_fragments(){
	for (int i = 0; i<FRAGMENT_AMOUNT; i++){
		if (fragment_is_falling[i]){
			draw_pixel(fragments_x[i],global_y-1,FG_COLOUR);
			draw_pixel(fragments_x[i]-1,global_y,FG_COLOUR);
			draw_pixel(fragments_x[i],global_y,FG_COLOUR);
			draw_pixel(fragments_x[i]+1,global_y,FG_COLOUR);
			draw_pixel(fragments_x[i],global_y+1,FG_COLOUR);
		}
	}
}

int get_global_y() { return global_y; }

char* get_asteroid_bitmap() { return (char*)asteroid_bitmap; }
int get_asteroid_x(int i) { return asteroids_x[i]; }
int is_asteroid_falling(int i) { return asteroid_is_falling[i]; }


char* get_boulder_bitmap() { return (char*)boulder_bitmap; }
int get_boulder_x(int i) { return boulders_x[i]; }
int is_boulder_falling(int i) { return boulder_is_falling[i]; }

char* get_fragment_bitmap() { return (char*)fragment_bitmap; }
int get_fragment_x(int i) { return fragments_x[i]; }
int is_fragment_falling(int i) { return fragment_is_falling[i]; }

int get_asteroids_amount() { return amount_asteroids; }
int get_boulders_amount() { return amount_boulders; }
int get_fragments_amount() { return amount_fragments; }