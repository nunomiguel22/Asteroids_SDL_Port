#include <SDL.h>
#include <time.h>


#include "renderer.h"
#include "game.h"


using namespace std;

int main(int argc, char *argv[]) {

	srand((unsigned int)time(NULL));

	game_data game;
	game.console.write_to_log("Initializing");
	game.console.write_to_log(" ");

	read_game_settings(&game.settings);

	game.gr_buffer = init_sdl(&game.settings, &game.console);

	if (game.gr_buffer == NULL) {
		game.console.save_log_to_file();
		return 1;
	}

	if (game_data_init(&game)) {
		game.console.save_log_to_file();
		return 1;
	}

	show_splash(&game);
	
	event_handler(&game);
	
	kill_sequence(&game);

	return 0;

}