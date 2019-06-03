#pragma once
#include <stdbool.h>

#define STARFIGHTER_WIDTH 10
#define STARFIGHTER_HEIGHT 5

#define BOLTS_AMOUNT 20


typedef enum { LEFT, RIGHT, STATION } dir;

void update_starfighter();

void setup_starfighter();

void draw_starfighter();

void draw_bolts();

void move_starfighter();

void shoot_bolt();

void change_direction(dir new_direction);

int starfighter_position();
void set_starfighter_position(int new_pos);

dir starfighter_direction();

int get_cannon_direction();

int get_bolt_x(int i);
int get_bolt_y(int i);

void clear_bolt(int i);

void decrease_lives(int amount);
void increase_score(int amount);
int get_lives();
void set_lives(int amount);
int get_score();
void set_score(int amount);

int get_bolts_amount();

bool no_lives();
