#include <SDL.h>
#include <time.h>


#include "renderer.h"
#include "game.h"


using namespace std;

int main(int argc, char *argv[]) {

	srand((unsigned int)time(NULL));

	game_data game;
	read_game_settings(&game.settings);

	game.gr_buffer = init_sdl(&game.settings);

	if (game_data_init(&game))
		return 1;


	show_splash(&game);
	
	event_handler(&game);

	free_sounds(&game.sound);
	exit_sdl();

	return 0;

}