#include <stdlib.h>

#include "ship.h"
#include "graphics.h"


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
	
	p->status = 0;
	p->status |= (BIT(7) | BIT(0));
	p->round = STARTING_ASTEROIDS;
	p->s_event = IDLING;


	for (int i = 0; i < AMMO; i++) {
		p->lasers[i].deactivate();
	}
}

void ship_mp_spawn(player *player1, player *player2, bool host) {

	player *left;
	player *right;
	if (host) {
		left = player1;
		right = player2;
	}
	else {
		left = player2;
		right = player1;

	}


	left->cannon.x = -300;
	left->cannon.y = 20;

	left->pivot.x = -300;
	left->pivot.y = 0;
	
	left->port.setX(-1);
	left->port.setY(0);

	left->starboard.setX(1);
	left->starboard.setY(0);

	left->force.setX(0);
	left->force.setY(0);

	left->crosshair.x = -100;
	left->crosshair.y = 0;

	right->cannon.x = 300;
	right->cannon.y = 20;


	right->pivot.x = 300;
	right->pivot.y = 0;

	right->port.setX(-1);
	right->port.setY(0);

	right->starboard.setX(1);
	right->starboard.setY(0);

	right->force.setX(0);
	right->force.setY(0);

	right->crosshair.x = 100;
	right->crosshair.y = 00;

	player1->status = 0;
	player1->hp = PLAYER_MAX_HEALTH;
	player1->hit_radius = SHIP_HITRADIUS;
	player1->status |= (BIT(7) | BIT(0));
	player1->s_event = IDLING;
	
	player2->status = 0;
	player2->hp = PLAYER_MAX_HEALTH;
	player2->hit_radius = SHIP_HITRADIUS;
	player2->status |= BIT(7);


	for (int i = 0; i < AMMO; i++) {
		player1->lasers[i].deactivate();
	}

	for (int i = 0; i < AMMO; i++) {
		player2->lasers[i].deactivate();
	}

}

void ship_mp_reset(player *player1, player *player2, bool host) {

	player *left;
	player *right;
	if (host) {
		left = player1;
		right = player2;
	}
	else {
		left = player2;
		right = player1;

	}


	left->cannon.x = -300;
	left->cannon.y = 20;

	left->pivot.x = -300;
	left->pivot.y = 0;

	left->port.setX(-1);
	left->port.setY(0);

	left->starboard.setX(1);
	left->starboard.setY(0);

	left->force.setX(0);
	left->force.setY(0);

	left->crosshair.x = -100;
	left->crosshair.y = 0;

	right->cannon.x = 300;
	right->cannon.y = 20;


	right->pivot.x = 300;
	right->pivot.y = 0;

	right->port.setX(-1);
	right->port.setY(0);

	right->starboard.setX(1);
	right->starboard.setY(0);

	right->force.setX(0);
	right->force.setY(0);

	right->crosshair.x = 100;
	right->crosshair.y = 00;

	player2->hp = PLAYER_MAX_HEALTH;
	player1->hp = PLAYER_MAX_HEALTH;
	player1->status |= (BIT(7) | BIT(0));
	player2->status |= (BIT(7) | BIT(0));

}


bool ship_mp_collision(player *player1, player *player2) {

	bool collision = false;

	/* Player1 laser to player2 ship collision */
	for (unsigned int j = 0; j < AMMO; j++) {
		if (player1->lasers[j].is_active()) {
			mpoint2d laserpos = player1->lasers[j].get_position();
			mvector2d v_ast_laser(laserpos, player2->pivot);

			if (v_ast_laser.magnitude() <= player2->hit_radius) {
				collision = true;
				player1->status |= BIT(2);
				player2->hp -= PLAYER_LASER_DAMAGE;
				player1->lasers[j].deactivate();
				//if (player2->hp <= 0) {

				//	timers->alien_death_timer = (unsigned int)(ALIEN_DEATH_DURATION * 60);
					//player2->hp = 100;
					//player2->active = false;
				//}
			}
		}
	}

	/* player2 laser to Player1 ship collision */
	for (unsigned int j = 0; j < AMMO; j++) {
		if (player2->lasers[j].is_active()) {
			mpoint2d laserpos = player2->lasers[j].get_position();
			mvector2d v_ast_laser(laserpos, player1->pivot);

			if (v_ast_laser.magnitude() <= player1->hit_radius) {
				collision = true;
				player1->hp -= PLAYER_LASER_DAMAGE;
				player2->lasers[j].deactivate();
				/*if (player1->hp <= 0) {
					//timers->alien_death_timer = (unsigned int)(ALIEN_DEATH_DURATION * 60);
					player1->hp = 100;
				}*/
			}
		}
	}

	/*  ship to ship collision */
	mvector2d v_ast_ship(player1->pivot, player2->pivot);
	float total_radius = player1->hit_radius + player2->hit_radius;
	if (total_radius > v_ast_ship.magnitude()) {
		collision = true;
		float x_sign_check = player1->force.getX()* player2->force.getX();
		float y_sign_check = player1->force.getY()* player2->force.getY();
		mvector2d temp;

		// Ships have roughly the same direction
		if (x_sign_check > 0 && y_sign_check > 0) {
			if (player1->force.magnitude() > player2->force.magnitude()) {
				player1->force *= 0.5;
				player2->force *= 2;
			}
			else if (player1->force.magnitude() < player2->force.magnitude()) {
				player1->force *= 0.5;
				player2->force *= 2;
			}
		}
		else {
			temp = player1->force;
			player1->force = player2->force;
			player2->force = temp;
		}
		
	}
	return collision;
}

void ship_warp(player *p) {

	/* Warps cannon point */
	if (p->cannon.x > math_h_positive_bound + 30)
		p->cannon.x -= hres + 30;

	else if (p->cannon.x < math_h_negative_bound - 30)
		p->cannon.x += hres + 30;


	if (p->cannon.y > math_v_positive_bound + 30)
		p->cannon.y -= vres + 30;

	else if (p->cannon.y < math_v_negative_bound - 30)
		p->cannon.y += vres + 30;

	/* Warps pivot point and rotates side versors */
	if (p->pivot.x > math_h_positive_bound + 30) {
		p->pivot.x -= hres + 30;
		p->port.rotate(180);
		p->starboard.rotate(180);
	}

	else if (p->pivot.x < math_h_negative_bound - 30) {
		p->pivot.x += hres + 30;
		p->port.rotate(180);
		p->starboard.rotate(180);
	}


	if (p->pivot.y > math_v_positive_bound + 30) {
		p->pivot.y -= vres + 30;
		p->port.rotate(180);
		p->starboard.rotate(180);

	}

	else if (p->pivot.y < math_v_negative_bound - 30) {
		p->pivot.y += vres + 30;
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

	float current_x = p->pivot.x;
	float current_y = p->pivot.y;
	if (random_xsign >= 5)
		random_x *= -1;
	if (random_ysign >= 5)
		random_y *= -1;

	p->pivot.x = (float)(random_x);
	p->pivot.y = (float)(random_y);
	p->cannon.x += random_x - current_x;

	p->cannon.y += random_y - current_y;
	p->status &= ~BIT(6);
	*timer = 0;

}

bool ship_apply_force(ship_event *s_event, player *p) {
	bool ship_move = false;

	/* Creates necessary versors */
	mvector2d vcannon (p->pivot, p->cannon);
	mvector2d cannon_versor = vcannon.versor();
	mvector2d port_th = p->port;
	mvector2d starboard_th = p->starboard;

	/* Applies force based on keyboard input and accelaration values in "macros.h" */
	switch (*s_event) {
		case MAIN_THRUSTER: {
			ship_move = true;
			cannon_versor *= MAIN_THRUSTER_ACCELARATION;
			p->force += cannon_versor;
			break;
		}
		case PORT_THRUSTER: {
			ship_move = true;
			port_th *= PORT_THRUSTER_ACCELARATION;
			p->force += port_th;
			break;
		}
		case STARBOARD_THRUSTER: {
			ship_move = true;
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
	return ship_move;
}

void ship_fire_laser(player *p, unsigned int *timer) {

	/* Activates an inactive laser from the laser struct, and gives it the same direction as the cannon versor */
	for (unsigned int i = 0; i < AMMO; i++)
		if (!p->lasers[i].is_active()) {
			mvector2d vcannon(p->pivot, p->cannon);
			mvector2d cannon_versor = vcannon.versor();
			
			cannon_versor *= LASER_VELOCITY;
			p->lasers[i].activate(p->cannon, cannon_versor, (float)(vcannon.angle() - 90));
			p->status &= ~BIT(7);

			//p->weapon_ready = false;
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
		float degrees = vmouse.angle() - vcannon.angle();
		vcannon.rotate(degrees);
		vcannon.limit(20);
		p->port.rotate(degrees);
		p->starboard.rotate(degrees);
	}

	p->cannon.x = p->pivot.x + vcannon.getX();
	p->cannon.y = p->pivot.y + vcannon.getY();

	/* Destroys out of bounds active lasers */
	for (unsigned int i = 0; i < AMMO; ++i) {
		if (p->lasers[i].is_active()) 
			p->lasers[i].update();
	}
}
