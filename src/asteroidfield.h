#pragma once
#include "ship.h"

/** @defgroup asteroidfield asteroidfield
* @{
* Functions related to asteroids
*/

/* Data structures */

/** @brief Asteroid sizes */
typedef enum { MEDIUM, LARGE } asteroid_size;

/** @brief Asteroid struct */
typedef struct {

	mpoint2d position;			/**< @brief Asteroid center x/y position */
	mpoint2d edge;				/**< @brief Asteroid edge point x/y position */
	mvector2d velocity;			/**< @brief Asteroid velocity vector */

	bool active;				/**< @brief Asteroid status */
	double hit_radius;			/**< @brief Asteroid hit radius for collisions */
	unsigned int death_timer;	/**< @brief Asteroid death timer for death animation */
	int degrees;				/**< @brief Asteroid rotation, for graphical purposes only */
	asteroid_size size;			/**< @brief Asteroid size */
}asteroid;

/* Functions */

/**
* @brief Initiates all large asteroids for the round with random movement, the number of asteroids increases with the number of rounds
*
* @param asteroid_field Array of asteroids
* @param player1 player1 struct
*/
void ast_spawn(asteroid asteroid_field[], player *player1);
/**
* @brief Checks for all asteroid related collisions
*
* @param asteroid_field Array of asteroids
* @param player1 Player1 struct
* @param alien Alien struct
*/
int ast_collision(asteroid asteroid_field[], player *player1, player *alien);
/**
* @brief Updates the position of all active asteroids, when crossing screen bounds warps asteroid to the other edge of the screen
*
* @param asteroid_field Array of asteroids
*/
void ast_update(asteroid asteroid_field[]);
/**
* @brief Fragments large asteroids into two smaller asteroids with random movement
*
* @param asteroid_field Array of asteroids
* @param ast_index Index of the large asteroid that was destroyed
*/
void ast_fragment(asteroid asteroid_field[], int ast_index);
