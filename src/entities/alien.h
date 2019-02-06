#pragma once
#include "ship.h"
#include "timers.h"

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
bool alien_update(player *alien, player *player1, game_timers *timers);
/**
* @brief Checks for all alien ship related collisions
*
* @param game General game struct
*/
void alien_collision(player *alien, player *player1, game_timers *timers);
