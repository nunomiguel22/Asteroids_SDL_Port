#include "ship.h"
#include "graphics.h"

#pragma warning(disable:4996)
/* GENERAL SHIP */

void ship_spawn(player *p) {

	/* Initiates ship's values */
	p->cannon.x = 30;
	p->cannon.y = 20;

	p->pivot.x = 30;
	p->pivot.y = 0;

	p->port.setX(-1);
	p->port.setY(0);

	p->starboard.setX(1);
	p->starboard.setY(0);

	p->force.setX(0);
	p->force.setY(0);

	p->crosshair.x = 30;
	p->crosshair.y = 50;

	p->hp = PLAYER_MAX_HEALTH;
	p->hit_radius = SHIP_HITRADIUS;
	p->score = 0;
	p->end_round = false;
	p->round = STARTING_ASTEROIDS;
	p->invulnerability = false;
	p->jump_ready = false;

	for (int i = 0; i < AMMO; i++) {
		p->lasers[i].active = false;
	}
}

void ship_warp(player *p) {

	/* Warps cannon point */
	if (p->cannon.x > math_h_positive_bound)
		p->cannon.x -= hres;

	else if (p->cannon.x < math_h_negative_bound)
		p->cannon.x += hres;


	if (p->cannon.y > math_v_positive_bound)
		p->cannon.y -= vres;

	else if (p->cannon.y < math_v_negative_bound)
		p->cannon.y += vres;

	/* Warps pivot point and rotates side versors */
	if (p->pivot.x > math_h_positive_bound) {
		p->pivot.x -= hres;
		p->port.rotate(180);
		p->starboard.rotate(180);
	}

	else if (p->pivot.x < math_h_negative_bound) {
		p->pivot.x += hres;
		p->port.rotate(180);
		p->starboard.rotate(180);
	}


	if (p->pivot.y > math_v_positive_bound) {
		p->pivot.y -= vres;
		p->port.rotate(180);
		p->starboard.rotate(180);

	}

	else if (p->pivot.y < math_v_negative_bound) {
		p->pivot.y += vres;
		p->port.rotate(180);
		p->starboard.rotate(180);
	}
}

void ship_teleport(player *p, unsigned int *timer) {

	/* Teleports ship to a random location avoiding edges, resets jump timer */
	int random_x = rand() % 400;
	int	random_y = rand() % 300;

	uint8_t random_xsign = rand() % 10;
	uint8_t random_ysign = rand() % 10;

	double current_x = p->pivot.x;
	double current_y = p->pivot.y;
	if (random_xsign >= 5)
		random_x *= -1;
	if (random_ysign >= 5)
		random_y *= -1;

	p->pivot.x = random_x;
	p->pivot.y = random_y;
	p->cannon.x += random_x - current_x;

	p->cannon.y += random_y - current_y;
	p->jump_ready = false;
	*timer = 0;

}

void ship_apply_force(ship_event *s_event, player *p) {

	/* Creates necessary versors */
	mvector2d vcannon (p->pivot, p->cannon);
	mvector2d cannon_versor = vcannon.versor();
	mvector2d port_th = p->port;
	mvector2d starboard_th = p->starboard;

	/* Applies force based on keyboard input and accelaration values in "macros.h" */
	switch (*s_event) {
		case MAIN_THRUSTER: {
			cannon_versor *= MAIN_THRUSTER_ACCELARATION;
			p->force += cannon_versor;
			break;
		}
		case PORT_THRUSTER: {
			port_th *= PORT_THRUSTER_ACCELARATION;
			p->force += port_th;
			break;
		}
		case STARBOARD_THRUSTER: {
			starboard_th *= STARBOARD_THRUSTER_ACCELARATION;
			p->force += starboard_th;
			break;
		}
		case REVERSE_THRUSTER: {
			cannon_versor *= MAIN_THRUSTER_ACCELARATION;
			p->force -= cannon_versor;
			break;
		}
		default: break;
	}

	/* Limits the force vector, and effectively the ship's velocity, to the maximum velocity value in "macros.h" */
	p->force.limit(THRUSTERS_MAXIMUM_VELOCITY);
}

void ship_fire_laser(player *p, unsigned int *timer) {

	/* Activates an inactive laser from the laser struct, and gives it the same direction as the cannon versor */
	for (unsigned int i = 0; i < AMMO; i++)
		if (!p->lasers[i].active) {
			mvector2d vcannon(p->pivot, p->cannon);
			mvector2d cannon_versor = vcannon.versor();

			cannon_versor *= LASER_VELOCITY;
			p->lasers[i].active = true;
			p->lasers[i].position = p->cannon;
			p->lasers[i].force = cannon_versor;
			p->weapon_ready = false;
			*timer = 0;
			break;
		}
}

void ship_update(player *p) {

	/* Updates ship's position */
	p->cannon.x += p->force.getX();
	p->cannon.y += p->force.getY();
	p->pivot.x += p->force.getX();
	p->pivot.y += p->force.getY();

	/* Warps ship to the other end when crossing bounds */
	ship_warp(p);

	 /* Rotates ship, avoids rotating when cursor is near the center of the ship */
	mvector2d vcannon(p->pivot, p->cannon);
	mvector2d vmouse(p->pivot, p->crosshair);

	if (vmouse.magnitude() > 5) {
		double degrees = vmouse.angle() - vcannon.angle();
		vcannon.rotate(degrees);
		vcannon.limit(20);
		p->port.rotate(degrees);
		p->starboard.rotate(degrees);
	}

	p->cannon.x = p->pivot.x + vcannon.getX();
	p->cannon.y = p->pivot.y + vcannon.getY();

	/* Destroys out of bounds active lasers */
	for (unsigned int i = 0; i < AMMO; i++) {
		if (p->lasers[i].active)
			p->lasers[i].position.x += p->lasers[i].force.getX();
		p->lasers[i].position.y += p->lasers[i].force.getY();
		if (p->lasers[i].position.x >= math_h_positive_bound || p->lasers[i].position.x <= math_h_negative_bound)
			p->lasers[i].active = false;
		if (p->lasers[i].position.y >= math_v_positive_bound || p->lasers[i].position.y <= math_v_negative_bound)
			p->lasers[i].active = false;
	}
}

/* ALIEN SHIP */

void alien_spawn(player *p) {

	/*Initiates alien ships values, gives it random movement */
	p->force.setX((double)rand() / RAND_MAX);
	p->force.setY((double)rand() / RAND_MAX);

	int random_xsign = rand() % 10;
	int random_ysign = rand() % 10;

	if (random_xsign >= 5) 
		p->force.setX(p->force.getX() * -1);

	if (random_ysign >= 5)
		p->force.setY(p->force.getY() * -1);

	p->pivot.x = rand() % math_h_positive_bound;
	p->pivot.y = rand() % math_v_positive_bound;

	p->cannon.x = p->pivot.x;
	p->cannon.y = p->pivot.y + 20;

	p->port.setX(-1);
	p->port.setY(0);

	p->starboard.setX(1);
	p->starboard.setY(0);

	p->hp = ALIEN_MAX_HEALTH;
	p->hit_radius = ALIEN_HIT_RADIUS;
	p->active = true;

	for (int i = 0; i < AMMO; i++) {
		p->lasers[i].active = false;
	}
}

void alien_update(player *alien, player *player1, game_timers *timers) {

	/* Updates alien ships position */
	alien->cannon.x += alien->force.getX();
	alien->cannon.y += alien->force.getY();
	alien->pivot.x += alien->force.getX();
	alien->pivot.y += alien->force.getY();

	/* Warps alien ship to the other end when crossing bounds */
	ship_warp(alien);

	/* Rotates ship, so it tracks the player's ship */
	mvector2d vcannon(alien->pivot, alien->cannon);
	mvector2d vships(alien->pivot, player1->pivot);

	double ship_degrees = vships.angle() - vcannon.angle();
	if (ship_degrees) {
		if (ship_degrees < 0)
			vcannon.rotate(-ALIEN_ROTATION_SPEED);
		else vcannon.rotate(+ALIEN_ROTATION_SPEED); 
	}
	vcannon.limit(20);
	alien->cannon.x = alien->pivot.x + vcannon.getX();
	alien->cannon.y = alien->pivot.y + vcannon.getY();

	/* Fires lasers when available */
	if (timers->alien_weapon_timer >= (60 / ALIEN_FIRE_RATE))
		ship_fire_laser(alien, &timers->alien_weapon_timer);

	/* Destroys out of bounds lasers */
	for (unsigned int i = 0; i < AMMO; i++) {
		if (alien->lasers[i].active)
			alien->lasers[i].position.x += alien->lasers[i].force.getX();
		alien->lasers[i].position.y += alien->lasers[i].force.getY();
		if (alien->lasers[i].position.x >= math_h_positive_bound || alien->lasers[i].position.x <= math_h_negative_bound)
			alien->lasers[i].active = false;
		if (alien->lasers[i].position.y >= math_v_positive_bound || alien->lasers[i].position.y <= math_v_negative_bound)
			alien->lasers[i].active = false;
	}
}

void alien_collision(player *alien, player *player1, game_timers *timers) {

	/* Player laser to alien ship collision */
	for (unsigned int j = 0; j < AMMO; j++) {
		if (player1->lasers[j].active) {
			mvector2d v_ast_laser(player1->lasers[j].position, alien->pivot);

			if (v_ast_laser.magnitude() <= alien->hit_radius) {
				alien->hp -= PLAYER_LASER_DAMAGE;
				player1->lasers[j].active = false;
				if (alien->hp <= 0) {
					timers->alien_death_timer = (unsigned int)(ALIEN_DEATH_DURATION * 60);
					alien->active = false;
					player1->score += 400;
				}
			}
		}
	}

	/* Alien laser to player ship collision */
	for (unsigned int j = 0; j < AMMO; j++) {
		if (alien->lasers[j].active) {
			mvector2d v_ast_laser(alien->lasers[j].position, player1->pivot);

			if (v_ast_laser.magnitude() <= player1->hit_radius) {
				player1->hp -= 20;
				alien->lasers[j].active = false;
			}
		}
	}

	/* Alien ship to player ship collision, this kills the player ship */
	mvector2d v_ast_ship(player1->pivot, alien->pivot);
	double total_radius = player1->hit_radius + alien->hit_radius;
	if (total_radius > v_ast_ship.magnitude())
		player1->hp -= player1->hp;
}


/* HIGHSCORES */

int load_highscores(unsigned int highscores[]) {

	/* Opens "highscores.txt" and loads highscores to the highscore array */
	FILE *fptr;
	fptr = fopen("highscores.txt", "r");
	if (fptr == NULL)
		return 0;

	for (int i = 0; i < 5; i++)
		fscanf(fptr, "%d", &highscores[i]);

	return 1;
}

void save_highscores(unsigned int highscores[]) {

	/* Opens "highscores.txt" and saves highscores to the file */
	FILE *fptr;

	fptr = fopen("highscores.txt", "w");
	for (int i = 0; i < 5; i++)
		fprintf(fptr, "%d\n", highscores[i]);

	fclose(fptr);
}

int verify_highscores(unsigned int highscores[], player *player1) {

	/* Check if score is a highscore */
	if (player1->score > highscores[4]) {
		highscores[4] = player1->score;

		/* Sort highscore array by highest to loweat if a new highscore is found */
		for (int i = 0; i < 5; i++) {
			for (int j = i + 1; j < 5; j++) {
				if (highscores[i] < highscores[j]) {
					unsigned int tempvar = highscores[i];
					highscores[i] = highscores[j];
					highscores[j] = tempvar;
				}
			}
		}
		/* Save highscores if a new highscore is found */
		save_highscores(highscores);
		return 1;
	}

	return 0;
}
