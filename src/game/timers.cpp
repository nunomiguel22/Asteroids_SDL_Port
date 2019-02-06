#include "timers.h"

void start_timers(game_timers *timers) {

	timers->framecounter = 1;
	timers->frames_per_second = 0;
	timers->cyclecounter = 0;
	timers->player1_weapon_timer = 0;
	timers->player2_weapon_timer = 0;
	timers->teleport_timer = 0;
	timers->round_timer = 0;
	timers->alien_death_timer = 0;
	timers->player1_death_timer = 0;
	timers->player2_death_timer = 0;
	timers->hitreg_timer = 0;
}

void increment_timers(game_timers *timers, asteroid asteroid_field[]) {

	for (unsigned int i = 0; i < MAX_ASTEROIDS; ++i)
		if (!asteroid_field[i].active && asteroid_field[i].death_timer > 0) {
			--asteroid_field[i].death_timer;
			if (asteroid_field[i].death_timer % 2 == 0)
				++asteroid_field[i].death_frame;
		}

	++timers->timerTick;
	++timers->player1_weapon_timer;
	++timers->player2_weapon_timer;
	++timers->alien_weapon_timer;
	++timers->teleport_timer;
	++timers->round_timer;
	if (timers->hitreg_timer > 0)
		--timers->hitreg_timer;
	if (timers->alien_death_timer > 0)
		--timers->alien_death_timer;
	if (timers->player1_death_timer > 0)
		--timers->player1_death_timer;
	if (timers->player2_death_timer > 0)
		--timers->player2_death_timer;
}