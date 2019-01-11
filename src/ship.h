#pragma once

#include "macros.h"
#include "mvector.h"

/** @defgroup Ship Ship
* @{
* Functions related to player controlled and enemy alien ships
*/

/* Data structures */

/** @brief Weapon struct for ship's lasers */
typedef struct {
	mpoint2d position; 			/**< @brief Laser x/y position */
	mvector2d force;				/**< @brief Vector with force applied to the laser's position */
	bool active;				/**< @brief Laser status */
}weapon;

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
	bool end_round;				/**< @brief True when current round is over */
	bool invulnerability;		/**< @brief True when ship is invulnerable */
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
void ship_apply_force(ship_event *s_event, player *p);
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

/* Alien ship */

/**
* @brief Initiates enemy alien ship values
*
* @param p Ship player struct
*/
void alien_spawn(player *p);
/**
* @brief Updates all alien ship physics: movement, rotation, fires lasers, warps ship
*
* @param game General game struct
*/
void alien_update(player *alien, player *player1, game_timers *timers);
/**
* @brief Checks for all alien ship related collisions
*
* @param game General game struct
*/
void alien_collision(player *alien, player *player1, game_timers *timers);

/* Highscores */

/**
* @brief Loads highscores from "highscores.txt" to the highscores array
*
* @param game General game struct
* @return Returns non-zero upon success and 0 otherwise
*/
int load_highscores(unsigned int highscores[]);
/**
* @brief Saves highscores to "highscores.txt" from the highscores array
*
* @param game General game struct
*/
void save_highscores(unsigned int highscores[]);
/**
* @brief Verifies if there is a new highscore, sorts by highscore and saves array
*
* @param game General game struct
* @return Returns 1 if the score is a highscore, 0 otherwise
*/
int verify_highscores(unsigned int highscores[], player *player1);