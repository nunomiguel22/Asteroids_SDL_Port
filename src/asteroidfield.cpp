#include "asteroidfield.h"
#include <stdlib.h>

void ast_spawn(asteroid asteroid_field[], player *player1) {

	/* Resets all asteroid death timers */
	for (unsigned int i = 0; i < MAX_ASTEROIDS; i++)
		asteroid_field[i].death_timer = 0;

	/* Initiates a number of asteroids based on round with random movement*/
	for (int i = 0; i < player1->round; i++) {

		/* Randomizes size based on round */
		int random_size = rand() % (100 - 1) + 1;
		if (random_size < (100 - ((player1->round - STARTING_ASTEROIDS) * SMALL_ASTEROID_CHANGE_INCREASE_RATE)))
			asteroid_field[i].size = LARGE;
		else asteroid_field[i].size = MEDIUM;

		asteroid_field[i].position.x = rand() % math_h_positive_bound;
		asteroid_field[i].position.y = rand() % math_v_positive_bound;

		/* Randomizes velocity/direction, smaller asteroids are always faster */
		if (asteroid_field[i].size == MEDIUM) {
			asteroid_field[i].velocity.setX(rand() % (MEDIUM_ASTEROID_MAX_VELOCITY - MEDIUM_ASTEROID_MIN_VELOCITY) + MEDIUM_ASTEROID_MIN_VELOCITY);
			asteroid_field[i].velocity.setY(rand() % (MEDIUM_ASTEROID_MAX_VELOCITY - MEDIUM_ASTEROID_MIN_VELOCITY) + MEDIUM_ASTEROID_MIN_VELOCITY);
		}
		else {
			asteroid_field[i].velocity.setX(rand() % (LARGE_ASTEROID_MAX_VELOCITY - LARGE_ASTEROID_MIN_VELOCITY) + LARGE_ASTEROID_MIN_VELOCITY);
			asteroid_field[i].velocity.setY(rand() % (LARGE_ASTEROID_MAX_VELOCITY - LARGE_ASTEROID_MIN_VELOCITY) + LARGE_ASTEROID_MIN_VELOCITY);
		}
		
		mvector2d random ((double)rand() / RAND_MAX, (double)rand() / RAND_MAX);
		asteroid_field[i].velocity += random;

		/* Randomizes position x/y sign*/
		int random_xsign = rand() % 10;
		int random_ysign = rand() % 10;
		if (random_xsign >= 5)
			asteroid_field[i].position.x *= -1;
		if (random_ysign >= 5)
			asteroid_field[i].position.y *= -1;

		/* Randomizes velocity x/y sign*/
		random_xsign = rand() % 10;
		random_ysign = rand() % 10;
		if (random_xsign >= 5)
			asteroid_field[i].velocity.setX(asteroid_field[i].velocity.getX() * -1);
		if (random_ysign >= 5)
			asteroid_field[i].velocity.setY(asteroid_field[i].velocity.getY() * -1);

		/* Asteroid hit radius based on size */
		if (asteroid_field[i].size == MEDIUM)
			asteroid_field[i].hit_radius = MEDIUM_ASTEROID_HITRADIUS;
		else asteroid_field[i].hit_radius = LARGE_ASTEROID_HITRADIUS;

		/* Activates asteroid and initiates graphical orientation*/
		asteroid_field[i].active = true;
		asteroid_field[i].degrees = 0;
	}

	/* Makes all not needed asteroids inactive */
	for (int i = player1->round; i < MAX_ASTEROIDS; i++) {
		asteroid_field[i].active = false;
	}
}

int ast_collision(asteroid asteroid_field[], player *player1, player *alien) {

	bool collision = false;

	for (unsigned int i = 0; i < MAX_ASTEROIDS; i++) {
		if (!asteroid_field[i].active && asteroid_field[i].death_timer > 0)
			asteroid_field[i].death_timer--;
	}

	player1->end_round = true;
	bool fragment = false;
	for (unsigned int i = 0; i < MAX_ASTEROIDS; i++) {
		if (asteroid_field[i].active) {

			/* Player laser to asteroid Collision */
			for (unsigned int j = 0; j < AMMO; j++) {

				if (player1->lasers[j].active) {

					mvector2d v_ast_laser(player1->lasers[j].position, asteroid_field[i].position);
					if (v_ast_laser.magnitude() <= asteroid_field[i].hit_radius) {
						collision = true;
						asteroid_field[i].active = false;
						player1->lasers[j].active = false;
						asteroid_field[i].death_timer = (unsigned int)(ASTEROID_DEATH_DURATION * 60);
						if (asteroid_field[i].size == LARGE) {
							player1->score += 50;
							ast_fragment(asteroid_field, i);
							fragment = true;
						}
						else {
							player1->score += 100;
						}
					}
				}
			}
			/* Player ship to asteroid Collision */
			mvector2d v_ast_ship(player1->pivot, asteroid_field[i].position);
			double total_radius = player1->hit_radius + asteroid_field[i].hit_radius;
			if (total_radius > v_ast_ship.magnitude()) {
				collision = true;
				asteroid_field[i].active = false;
				if (asteroid_field[i].size == LARGE) {
					if (!player1->invulnerability) {
						player1->hp -= 30;
						player1->score += 50;
					}
				}
				else {
					if (!player1->invulnerability) {
						player1->hp -= 15;
						player1->score += 100;
					}
				}
				asteroid_field[i].death_timer = (unsigned int)(ASTEROID_DEATH_DURATION * 60);
			}

			/* End round when all asteroids are destroyed */
			if (asteroid_field[i].active || asteroid_field[i].death_timer > 0 || fragment)
				player1->end_round = false;
		}
		/* Do not end round if the alien ship is still alive */
		if (alien->active)
			player1->end_round = false;

	}
	return collision;

}

void ast_update(asteroid asteroid_field[]) {

	/* Updates asteroid position and warps asteroids to the other edge of the screen */
	for (unsigned int i = 0; i < MAX_ASTEROIDS; i++) {
		if (asteroid_field[i].active) {

			asteroid_field[i].position.x += asteroid_field[i].velocity.getX();
			asteroid_field[i].position.y += asteroid_field[i].velocity.getY();

			if (asteroid_field[i].position.x > math_h_positive_bound)
				asteroid_field[i].position.x -= hres;
			else if (asteroid_field[i].position.x < math_h_negative_bound)
				asteroid_field[i].position.x += hres;

			if (asteroid_field[i].position.y > math_v_positive_bound)
				asteroid_field[i].position.y -= vres;
			else if (asteroid_field[i].position.y < math_v_negative_bound)
				asteroid_field[i].position.y += vres;

			if (asteroid_field[i].degrees >= 360)
				asteroid_field[i].degrees -= 360;
			else asteroid_field[i].degrees++;
		}
	}
}

void ast_fragment(asteroid asteroid_field[], int ast_index) {

	/* Fragments large asteroids into two smaller asteroids with random movement */
	int frag_counter = 0;
	double x = asteroid_field[ast_index].position.x;
	double y = asteroid_field[ast_index].position.y;

	for (int i = 0; i < MAX_ASTEROIDS; i++) {

		if (!asteroid_field[i].active && asteroid_field[i].death_timer == 0) {
			frag_counter++;
			asteroid_field[i].size = MEDIUM;

			asteroid_field[i].position.x = x;
			asteroid_field[i].position.y = y;

			asteroid_field[i].velocity.setX (rand() % (MEDIUM_ASTEROID_MAX_VELOCITY - MEDIUM_ASTEROID_MIN_VELOCITY) + MEDIUM_ASTEROID_MIN_VELOCITY);
			asteroid_field[i].velocity.setY (rand() % (MEDIUM_ASTEROID_MAX_VELOCITY - MEDIUM_ASTEROID_MIN_VELOCITY) + MEDIUM_ASTEROID_MIN_VELOCITY);
			
			mvector2d random((double)rand() / RAND_MAX, (double)rand() / RAND_MAX);
			asteroid_field[i].velocity += random;

			int random_xsign = rand() % 10;
			int random_ysign = rand() % 10;
			if (random_xsign >= 5)
				asteroid_field[i].velocity.setX(asteroid_field[i].velocity.getX() * -1);
			if (random_ysign >= 5)
				asteroid_field[i].velocity.setY(asteroid_field[i].velocity.getY() * -1);


			asteroid_field[i].hit_radius = MEDIUM_ASTEROID_HITRADIUS;
			asteroid_field[i].active = true;
			asteroid_field[i].degrees = 0;
		}

		if (frag_counter == 2)
			break;
	}
}
