#include <SDL.h>
#include <Windows.h>
#include "vcard.h"
#include "graphics.h"
#include "game.h"
#include <time.h>

using namespace std;

int main(int argc, char *argv[]) {

	srand(time(NULL));

	game_data game;

	game.gr_buffer = init_sdl();

	if (game_data_init(&game))
		return 1;


	show_splash(&game);
	
	event_handler(&game);

	return 0;

}