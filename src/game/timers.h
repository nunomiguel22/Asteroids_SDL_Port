#pragma once

#include "asteroidfield.h"

/** @brief Various game related timers */
typedef struct {
	unsigned int framecounter;				/**< @brief Counts every frame displayed on screen */
	unsigned int frames_per_second;			/**< @brief Saves the amount of frames displayed every second */
	unsigned int cyclecounter;				/**< @brief Counts the number of cycles done while playing */
	unsigned int timerTick;					/**< @brief Counts the number of timer 0 interrupts */
	unsigned int player1_weapon_timer;		/**< @brief Controls player1 fire rate */
	unsigned int player1_death_timer;
	unsigned int player2_weapon_timer;		/**< @brief Controls player2 fire rate */
	unsigned int player2_death_timer;
	unsigned int alien_weapon_timer;		/**< @brief Controls alien fire rate */
	unsigned int alien_death_timer;			/**< @brief Used for alien death animation */
	unsigned int teleport_timer;			/**< @brief Controls player1 teleport availability rate */
	unsigned int round_timer;				/**< @brief Counts time passed in round */
	unsigned int hitreg_timer;
	int start_seq;							/**< @brief Single player start sequence countdown */
}game_timers;

/**
* @brief Initiates/resets all game timers
*
* @param timers Game timers
*/
void start_timers(game_timers *timers);
/**
* @brief Increments all game timers
*
* @param timers Game timers
*/
void increment_timers(game_timers *timers, asteroid asteroid_field[]);
