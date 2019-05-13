#include <windows.h>
#include <ShellApi.h>
#include "../game/game.h"
#include "../renderer/renderer.h"

void main_menu_handler(game_data *game){

	static bool first_frame = false;
	/* Operations when entering menu: start music, initiate menu asteroids, etc */
	if (!first_frame) {
		if (game->settings.music_volume)
			game->threads.push(play_music, std::ref(*game->sound.galaxia));
		first_frame = true;
		game->alien.round = 7;
		ast_spawn(game->menu_asteroid_field, &game->alien);
		handle_menu_frame(game, &game->bmp.menubackground);
		game->timers.timerTick = 0;
	}

	if (game->event == KEYBOARD) {
		/* Toggle console */
		if (game->SDLevent.key.keysym.sym == SDLK_BACKSLASH || game->SDLevent.key.keysym.sym == SDLK_BACKQUOTE)
			game->console.toggle();
		/* Console input */
		if (game->console.is_open()) {
			std::string cmd;
			cmd = game->console.input_handler(game->SDLevent);
			if (!cmd.empty())
				exec_console_cmd(cmd, game);
		}
	}
	static int menu_pos = 0;
	if (game->event == MOUSE)
		//Check menu selection

		if (game->SDLevent.motion.x >= 395 && game->SDLevent.motion.x <= 573) {

			if (game->SDLevent.motion.y >= 200 && game->SDLevent.motion.y <= 228) {
				if (menu_pos != 1) {
					game->threads.push(play_sound, std::ref(*game->sound.menu_tick));
					menu_pos = 1;
				}
				if (game->SDLevent.button.button == SDL_BUTTON_LEFT && game->timers.timerTick >= 30) {
					game->threads.push(stop_music);
					game->state = SP_START_SEQUENCE;
					first_frame = false;
					return;
				}
			}
			else if (game->SDLevent.motion.y >= 400 && game->SDLevent.motion.y <= 428) {
				if (menu_pos != 6) {
					game->threads.push(play_sound, std::ref(*game->sound.menu_tick));
					menu_pos = 6;
				}
				if (game->timers.timerTick >= 30 && game->SDLevent.button.button == SDL_BUTTON_LEFT) {
					game->state = COMP;
					return;
				}

			}
			else if (game->SDLevent.motion.y >= 320 && game->SDLevent.motion.y <= 348) {
				if (menu_pos != 4) {
					game->threads.push(play_sound, std::ref(*game->sound.menu_tick));
					menu_pos = 4;
				}
				if (game->timers.timerTick >= 30 && game->SDLevent.button.button == SDL_BUTTON_LEFT) {
					game->state = OPTIONSMENU;
					return;
				}
			}
			else if (game->SDLevent.motion.y >= 240 && game->SDLevent.motion.y <= 268) {
				if (menu_pos != 2) {
					game->threads.push(play_sound, std::ref(*game->sound.menu_tick));
					menu_pos = 2;
				}
				if (game->timers.timerTick >= 30 && game->SDLevent.button.button == SDL_BUTTON_LEFT) {
					exec_console_cmd("init_server 1337", game);
					exec_console_cmd("host_game", game);
					return;
				}
			}
			else if (game->SDLevent.motion.y >= 280 && game->SDLevent.motion.y <= 308) {
				if (menu_pos != 3) {
					game->threads.push(play_sound, std::ref(*game->sound.menu_tick));
					menu_pos = 3;
				}
				if (game->timers.timerTick >= 30 && game->SDLevent.button.button == SDL_BUTTON_LEFT) {
					game->console.set_command("connect ");
					game->console.open();
					return;
				}
			}
			else if (game->SDLevent.motion.y >= 360 && game->SDLevent.motion.y <= 388) {
				if (menu_pos != 5) {
					game->threads.push(play_sound, std::ref(*game->sound.menu_tick));
					menu_pos = 5;
				}
				if (game->timers.timerTick >= 30 && game->SDLevent.button.button == SDL_BUTTON_LEFT) {
					ShellExecute(0, 0, "https://github.com/nunomiguel22/Asteroids_SDL_Port", 0, 0, SW_SHOW);


					return;
				}
			}


			else menu_pos = 0;
			
		}
		else menu_pos = 0;

		

	if (game->event == TIMER) {
		ast_update(game->menu_asteroid_field);
		handle_menu_frame(game, &game->bmp.menubackground);
	}



}





