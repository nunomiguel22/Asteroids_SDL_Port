#include <time.h>


#include "renderer.h"
#include "game.h"

/*
	This project requires SDL and SDL Libraries SDLNet, SDL_TTF, SDL_MIXER and the CTPL Library.

*/


int main(int argc, char *argv[]) {

	srand((unsigned int)time(NULL));
	
	game_data game;

	game.console.log_push("Initializing");
	game.console.log_push(" ");

	read_game_settings(&game.settings);

	game.gr_buffer = init_sdl(&game.settings, &game.console);

	if (game.gr_buffer == NULL) {
		game.console.log_save();
		return 1;
	}

	if (game_data_init(&game)) {
		game.console.log_save();
		return 1;
	}

	show_splash(&game);
	
	event_handler(&game);
	
	kill_sequence(&game);
	
	return 0;
	
}
