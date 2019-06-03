#include <stdio.h>
#include <stdlib.h>
#include "collision.h"
#include "starfighter.h"
#include "falling_objects.h"

#include <stdbool.h>

const char* bolt_bitmap = 
".."
"..";

const int w0 = 2;
const int h0 = 2;


bool pixel_collision(int x0, int y0, int w0, int h0, char* pixels0, int x1, int y1, int w1, int h1, char* pixels1){
    for (int j = 0; j < h0; j++){
        for (int i = 0; i < w0; i++){
            if (pixels0[i + j*w0] != ' '){ // ignores '?' pixel
                int x = x0 + i;
                int y = y0 + j;
                if (
                    x >= x1 &&
                    x < x1 + w1 &&
                    y >= y1 &&
                    y < y1 + h1 &&
                    pixels1[(x-x1)+(y-y1)*w1] != ' ' // ignore '?' pixel
                    ) {
                    return true;
                }
            }
        }
    }
    return false;
}

int shoots_asteroid(){

	char* asteroid_bitmap = get_asteroid_bitmap();
	int w1 = ASTEROID_SIZE;
	int h1 = ASTEROID_SIZE;
	int y1 = get_global_y()-ASTEROID_SIZE/2;

	for (int i = 0; i < BOLTS_AMOUNT; i++){
		for (int j = 0; j < ASTEROID_AMOUNT; j++){
			if (is_asteroid_falling(j)){
				int x0 = get_bolt_x(i)-1;
				int y0 = get_bolt_y(i)-1;
				
				int x1 = get_asteroid_x(j)-ASTEROID_SIZE/2;

				if (pixel_collision(x0, y0, w0, h0, (char*)bolt_bitmap, x1, y1, w1, h1, asteroid_bitmap)){
					clear_bolt(i);
					break_asteroid(j);
					increase_score(ASTEROID_POINTS);
					return 0;
				}
			}
		}
	}
	return 0;
}

int shoots_boulder(){

	char* boulder_bitmap = get_boulder_bitmap();
	int w1 = BOULDER_SIZE;
	int h1 = BOULDER_SIZE;
	int y1 = get_global_y()-BOULDER_SIZE/2;

	for (int i = 0; i < BOLTS_AMOUNT; i++){
		for (int j = 0; j < BOULDER_AMOUNT; j++){
			if (is_boulder_falling(j)){
				int x0 = get_bolt_x(i)-1;
				int y0 = get_bolt_y(i)-1;
				
				int x1 = get_boulder_x(j)-BOULDER_SIZE/2;

				if (pixel_collision(x0, y0, w0, h0, (char*)bolt_bitmap, x1, y1, w1, h1, boulder_bitmap)){
					clear_bolt(i);
					break_boulder(j);
					increase_score(BOULDER_POINTS);
					return 0;
				}
			}
		}
	}
	return 0;
}

int shoots_fragment(){

	char* fragment_bitmap = get_fragment_bitmap();
	int w1 = FRAGMENT_SIZE;
	int h1 = FRAGMENT_SIZE;
	int y1 = get_global_y()-FRAGMENT_SIZE/2;

	for (int i = 0; i < BOLTS_AMOUNT; i++){
		for (int j = 0; j < FRAGMENT_AMOUNT; j++){
			if (is_fragment_falling(j)){
				int x0 = get_bolt_x(i)-1;
				int y0 = get_bolt_y(i)-1;
				
				int x1 = get_fragment_x(j)-FRAGMENT_SIZE/2;

				if (pixel_collision(x0, y0, w0, h0, (char*)bolt_bitmap, x1, y1, w1, h1, fragment_bitmap)){
					clear_bolt(i);
					break_fragment(j);
					increase_score(FRAGMENT_POINTS);
					return 0;
				}
			}
		}
	}
	return 0;
}


void do_collisions(){
	shoots_asteroid();
	shoots_boulder();
	shoots_fragment();
}



