#include <SDL.h>
#include <Windows.h>
#include "vcard.h"
#include "graphics.h"
#include "game.h"
#include <time.h>

using namespace std;

int main(int argc, char *argv[]) {

	srand(time(NULL));
	video_settings *settings = (video_settings *) malloc (sizeof(video_settings));
	game_data game;
	read_video_settings(settings);

	game.gr_buffer = init_sdl(settings);

	if (game_data_init(&game))
		return 1;


	show_splash(&game);
	
	event_handler(&game);

	exit_sdl();

	return 0;

}