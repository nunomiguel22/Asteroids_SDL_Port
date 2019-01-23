#pragma once

#include "macros.h"
#include "mvector.h"
#include "weapon.h"

/** @defgroup Ship Ship
* @{
* Functions related to player controlled and enemy alien ships
*/

/* Data structures */

/** @brief Weapon struct for ship's lasers */
//typedef struct {
//	mpoint2d position; 			/**< @brief Laser x/y position */
//	mvector2d force;				/**< @brief Vector with force applied to the laser's position */
//	bool active;				/**< @brief Laser status */
//}weapon;




/** @brief Player struct */
typedef struct {
	mpoint2d cannon;				/**< @brief Cannon x/y position */
	mpoint2d pivot;				/**< @brief Pivot (center of ship) x/y position */
	mvector2d port;				/**< @brief Versor with port engine direction */
	mvector2d starboard;			/**< @brief Versor with starboard engine direction */
	mvector2d force;				/**< @brief Vector with force applied to the ship's position */
	mpoint2d crosshair;			/**< @brief Crosshair x/y position */

	int hp;						/**< @brief Ships current health points */
	unsigned int score;			/**< @brief Player1 single player score */
	double hit_radius;			/**< @brief Ship's hit radius for collisions */
	int round;					/**< @brief Current single player round */
	weapon lasers[AMMO];		/**< @brief Laser array */
	bool weapon_ready;			/**< @brief True when ship is ready to fire */
	bool jump_ready;			/**< @brief True when ship is ready to teleport */
	bool teleporting;
	unsigned int teleport_time;
	bool end_round;				/**< @brief True when current round is over */
	bool invulnerability;		/**< @brief True when ship is invulnerable */
	bool hit_reg;
	bool active;				/**< @brief Ship's status, used for alien ship */
}player;


/* Functions */

/**
* @brief Initiates ship values for single player mode
*
* @param p Ship player struct
*/
void ship_spawn(player *p);
/**
* @brief When crossing screen bounds warps ship to the other edge of the screen
*
* @param p Ship player struct
*/
void ship_warp(player *p);
/**
* @brief Teleports ship to a random location
*
* @param p Ship player struct
* @param timer teleport timer
*/
void ship_teleport(player *p, unsigned int *timer);
/**
* @brief Updates the ship's force vector based on a keyboard input
*
* @param p Ship player struct
* @param keyboard_event Keyboard input
*/
bool ship_apply_force(ship_event *s_event, player *p);
/**
* @brief Updates the ship's laser struct to fire a laser in the cannon's direction
*
* @param p Ship player struct
* @param timer Fire rate timer
*/
void ship_fire_laser(player *p, unsigned int *timer);
/**
* @brief Applies the force vector to the ship's position, warps and rotates ship, destroys out of bound lasers
*
* @param p Ship player struct
*/
void ship_update(player *p);
