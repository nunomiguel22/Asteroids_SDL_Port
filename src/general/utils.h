#pragma once

#include "ship.h"

typedef struct {
	uint32_t hresolution;
	uint32_t vresolution;
	bool fullscreen;
	bool fullscreennative;
	bool borderless;
	bool vsync;
	bool fps_counter;
	int fps;
	int effects_volume;
	int music_volume;
	std::string name;
}game_settings;

void read_game_settings(game_settings *settings);
void reset_game_settings(game_settings *settings);
void save_game_settings(game_settings *settings);

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

