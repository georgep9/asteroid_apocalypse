#pragma once

#define ASTEROID_AMOUNT 3
#define ASTEROID_SIZE 7
#define ASTEROID_POINTS 1

#define BOULDER_AMOUNT 6
#define BOULDER_SIZE 5
#define BOULDER_POINTS 2

#define FRAGMENT_AMOUNT 12
#define FRAGMENT_SIZE 3
#define FRAGMENT_POINTS 4


void setup_objects();

void update_objects();

void draw_objects();

int get_global_y();



char* get_asteroid_bitmap();
int get_asteroid_x(int i);
int is_asteroid_falling(int i);
void break_asteroid(int i);

char* get_boulder_bitmap();
int get_boulder_x(int i);
int is_boulder_falling(int i);
void break_boulder(int i);

char* get_fragment_bitmap();
int get_fragment_x(int i);
int is_fragment_falling(int i);
void break_fragment(int i);

int get_asteroids_amount();
int get_boulders_amount();
int get_fragments_amount();