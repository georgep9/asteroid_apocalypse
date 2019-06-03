#pragma once

#define FREQ (8000000.0)
#define PRESCALE (1024.0)

void setup_status();

void display_status();

void send_status();


double get_game_time();

double get_time_count();

void pause_game();
void unpause_game();

void enable_show_status();