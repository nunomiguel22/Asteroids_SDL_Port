#pragma once
#include <stdint.h>
#include <string>

#include "macros.h"
#include "mvector.h"
#include "weapon.h"


/* Data structures */

/** @brief Ship events */
typedef enum { MAIN_THRUSTER, PORT_THRUSTER, STARBOARD_THRUSTER, REVERSE_THRUSTER, K_ESC, QUIT, IDLING } ship_event;

/* Ship struct. 216 bytes, 2 bytes of padding space at the end */
typedef struct {
	mpoint2d cannon;				// Cannon x/y position 
	mpoint2d pivot;					// Pivot (center of ship) x/y position 
	mvector2d port;					// Versor with port engine direction 
	mvector2d starboard;			// Versor with starboard engine direction 
	mvector2d force;				// Vector with force applied to the ship's position 
	mpoint2d crosshair;				// Crosshair x/y position 

	weapon lasers[AMMO];			// Laser array 

	ship_event s_event;
	int16_t hp;						// Ships current health points 
	int16_t round;					// Current single player round 
	unsigned int score;				// Player1 single player score 
	float hit_radius;				// Ship's hit radius for collisions 
	unsigned int teleport_time;		// Teleport animation timer

	/*
	BIT 7 - Weapon ready: 1 if weapon is ready to fire
	BIT 6 - Jump ready: 1 if ship is ready to teleport
	BIT 5 - Teleporting: 1 if ship is currently teleporting
	BIT 4 - End round: 1 if singleplayer round is over
	BIT 3 - Invulnerability: 1 if ship is invulnerable
	BIT 2 - Hit Reg: 1 if ship's lasers hit a target
	BIT 1 - MP ready: 1 if player is ready on a multiplayer game
	BIT 0 - active: 1 if this ship is active
	*/
	std::string name;
	uint8_t status;
	uint8_t mp_round;
	// 2 Bytes of padding space
}player;


/* Functions */


void ship_spawn(player *p);

void ship_mp_spawn(player *player1, player *player2, bool host);

void ship_mp_reset(player *player1, player *player2, bool host);

bool ship_mp_collision(player *player1, player *player2);

void ship_warp(player *p);

void ship_teleport(player *p, unsigned int *timer);

bool ship_apply_force(ship_event *s_event, player *p);

void ship_fire_laser(player *p, unsigned int *timer);

void ship_update(player *p);

