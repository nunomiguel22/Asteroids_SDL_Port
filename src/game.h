#pragma once
#include <ctpl_stl.h>
#include <SDL.h>
#include <SDL_mixer.h>

#include "mvector.h"
#include "graphics.h"
#include "macros.h"
#include "ship.h"
#include "asteroidfield.h"
#include "sound.h"
#include "utils.h"

typedef enum { KEYBOARD, MOUSE, TIMER} event_type;

/** @brief Enum with all gamestates */
typedef enum { MENU, OPTIONSMENU, START_SEQUENCE, PLAYING, NEW_ROUND, LOSS, GAMEPAUSED, CONNECTING, COMP } game_sts;

/** @brief Main game data structure */
typedef struct {
	ctpl::thread_pool threads;						/**< @brief Processor threads */
	game_sts state;									/**< @brief Current game state */
	game_timers timers;								/**< @brief Struct with the game's timers */
	player player1;									/**< @brief Player1, this is always the ship controlled by this computer */
	player alien;
	asteroid menu_asteroid_field[MAX_ASTEROIDS];
	asteroid asteroid_field[MAX_ASTEROIDS];			/**< @brief Asteroid array */
	unsigned int highscores[5];						/**< @brief Highscores array, up to 5 highscores */

	SDL_Event SDLevent;								/**< @brief Type of input event */
	event_type event;
	ship_event s_event;

	uint8_t *gr_buffer;								/**< @brief Pointer to local pixel buffer */
	char FILEPATH[50];								/**< @brief Filepath to game folder */
	bitmap_data bmp;								/**< @brief All bmps images */
	pixmap_data xpm;								/**< @brief All pixmaps */
	sound_data sound;
	game_settings settings;							/**< @brief Options menu's settings */

} game_data;


/**
* @brief Initiates/resets all game data
*
* @param game General game struct
* @returns 0 if successful, 1 otherwise
*/
int game_data_init(game_data *game);
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
void increment_timers(game_timers *timers);
/**
* @brief This function does all singleplayer physics operations
*
* @param game General game struct
*/
void physics_update(int id, game_data &game);
/**
* @brief Main loop, handles all interrupt events
*
* @param game General game struct
*/
void event_handler(game_data* game);
/**
* @brief Main game state machine
*
* @param game General game struct
*/
void game_state_machine(game_data* game);
/**
* @brief Handles events when in singleplayer mode
*
* @param game General game struct
*/
void playing_event_handler(game_data* game);


