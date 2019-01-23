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
	p->end_round = false;
	p->round = STARTING_ASTEROIDS;
	p->invulnerability = false;
	p->jump_ready = false;
	p->hit_reg = false;
	p->teleporting = false;

	for (int i = 0; i < AMMO; i++) {
		p->lasers[i].setstatus(false);
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
		if (!p->lasers[i].active()) {
			mvector2d vcannon(p->pivot, p->cannon);
			mvector2d cannon_versor = vcannon.versor();

			cannon_versor *= LASER_VELOCITY;
			p->lasers[i].setstatus(true);
			p->lasers[i].setposition (p->cannon);
			p->lasers[i].setforce (cannon_versor);
			p->lasers[i].set_travel_angle((float)(vcannon.angle() - 90));
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
		if (p->lasers[i].active()) {

			p->lasers[i].updateposition();
			p->lasers[i].checkbounds();
		}
	}
}

