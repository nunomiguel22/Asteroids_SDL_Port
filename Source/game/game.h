#pragma once
#include <ctpl_stl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "timers.h"
#include "../general/mvector.h"
#include "../renderer/graphics.h"
#include "../general/macros.h"
#include "../entities/ship.h"
#include "../entities/asteroidfield.h"
#include "../general/sound.h"
#include "../general/utils.h"
#include "../general/console.h"
#include "../general/netcode.h"


typedef enum { KEYBOARD, MOUSE, TIMER, EV_IDLE} event_type;

/** @brief Enum with all gamestates */
typedef enum { MENU, OPTIONSMENU, SP_START_SEQUENCE, SINGLEPLAYER, SP_NEW_ROUND, SP_LOSS, SP_GAMEPAUSED,
	MP_WARMUP, MP_START_SEQUENCE, MP_ROUND,  COMP } game_states;

/** @brief Main game data structure */
typedef struct {
	ctpl::thread_pool threads;						/**< @brief Processor threads */
	game_states state;									/**< @brief Current game state */
	game_timers timers;								/**< @brief Struct with the game's timers */
	console console;
	UDPnet connection;
	player player1;									/**< @brief Player1, this is always the ship controlled by this computer */
	player player2;
	int ping;
	bool host;
	player alien;
	asteroid menu_asteroid_field[MAX_ASTEROIDS];
	asteroid asteroid_field[MAX_ASTEROIDS];			/**< @brief Asteroid array */
	unsigned int highscores[5];						/**< @brief Highscores array, up to 5 highscores */

	SDL_Event SDLevent;								/**< @brief Type of input event */
	event_type event;

	uint32_t *gr_buffer;							/**< @brief Pointer to local pixel buffer */
	bitmap_data bmp;								/**< @brief All bmps images */
	font_data ttf_fonts;							/**< @brief All ttf fonts */
	sound_data sound;								/**< @brief All game sounds */
	game_settings settings;							/**< @brief Options menu's settings */

} game_data;

/* game.cpp */

void event_handler(game_data *game);
void game_state_machine(game_data* game);

/* gameaux.cpp */

int game_data_init(game_data *game);
void kill_sequence(game_data *game);
void physics_update(int id, game_data &game);
void exec_console_cmd(std::string cmd, game_data *game);



/* singleplayer.cpp */

void singleplayer_event_handler(game_data *game);
void singleplayer_start_sequence(game_data *game);
void singleplayer_game_over(game_data *game);
void singleplayer_pause(game_data *game);
void mouse_handler(int id, game_data &game);
void new_round_reset(game_data *game);



/* User interface */

void main_menu_handler(game_data *game);
void options_menu_handler(game_data *game);
void options_button_check(game_data *game);


void start_listening(int id, game_data &game); 
void start_transmitting(int id, game_data &game);
void physics_mp_update(int id, game_data &game);
void multiplayer_event_handler(game_data* game);
void multiplayer_start_sequence(game_data *game);
