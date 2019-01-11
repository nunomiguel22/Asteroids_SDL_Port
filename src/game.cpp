#include "game.h"
#include "SDL.h"
#include "vcard.h"
#include <iostream>

void handle_menu_frame(game_data *game, Bitmap *bckgrd) {

	bckgrd->draw(0, 0);

	switch (game->state) {
	case MENU: {
		for (int i = 0; i < 5; i++)
			draw_number(game->highscores[i], 900, 60 + i * 40, game);
		break;
	}
	case OPTIONSMENU: {
		if (game->settings.fps_counter)
			game->bmp.boxticked.draw(369, 173);

		if (!game->settings.fps)
			game->bmp.boxticked.draw(870, 74);
		else if (game->settings.fps == 1)
			game->bmp.boxticked.draw(661, 75);
		else game->bmp.boxticked.draw(548, 77);


		if (game->settings.m_sens < 1)
			game->bmp.boxticked.draw(583, 99);
		else if (game->settings.m_sens == 1)
			game->bmp.boxticked.draw(712, 100);
		else game->bmp.boxticked.draw(838, 100);

		break;
	}
	default: break;

	}

	game->xpm.cursor.draw(game->SDLevent.motion.x + 5, game->SDLevent.motion.y + 7, false);

	display_frame();
}

void start_timers(game_timers *timers) {

	timers->framecounter = 1;
	timers->frames_per_second = 0;
	timers->cyclecounter = 0;
	timers->player1_weapon_timer = 0;
	timers->player2_weapon_timer = 0;
	timers->teleport_timer = 0;
	timers->round_timer = 0;
	timers->alien_death_timer = 0;
}



int game_data_init(game_data *game) {

	if (!load_highscores(game->highscores))
		for (int i = 0; i < 5; i++)
			game->highscores[i] = 0;

	game->timers.timerTick = 0;
	game->timers.start_seq = 3;
	game->state = MENU;
	game->timers.cyclecounter = 0;

	game->settings.fps_counter = true;
	game->settings.fps = 1;
	game->settings.m_sens = 1;
	//game->alien.active = false;

	load_xpms(&game->xpm);
	if (load_bitmaps(&game->bmp))
		return 1;
	return 0;
}

void event_handler(game_data* game) {

	double lastTick = SDL_GetTicks();
	
	while (game->state != COMP) {

		double currentTick = SDL_GetTicks();
		
		if(SDL_PollEvent(&game->SDLevent)) {
			switch (game->SDLevent.type) {
				case SDL_MOUSEMOTION: {
					game->event = MOUSE;
					game_state_machine(game);
					break;
				}
				case SDL_MOUSEBUTTONDOWN: {
					game->event = MOUSE;
					game_state_machine(game);

					break;
				}
				case SDL_KEYDOWN: {
					game->event = KEYBOARD;
					game_state_machine(game);

					break;
				}

			}

		}

		if (currentTick - lastTick >= 17) {

			game->timers.timerTick++;
			std::cout << game->timers.timerTick << std::endl;
			game->timers.player1_weapon_timer++;
			game->timers.player2_weapon_timer++;
			game->timers.alien_weapon_timer++;
			game->timers.teleport_timer++;
			game->timers.round_timer++;
			if (game->timers.alien_death_timer > 0)
				game->timers.alien_death_timer--;
			game->event = TIMER;
			game_state_machine(game);
			lastTick = currentTick;
		}
	}
}


void game_state_machine(game_data* game) {

	switch (game->state) {

		case MENU: {
			static bool first_frame = false;
			if (!first_frame) {
				first_frame = true;
				handle_menu_frame(game, &game->bmp.menu);
				game->timers.timerTick = 0;
			}

			if (game->event == MOUSE) {

				//checking button clicks
				if (game->SDLevent.button.button == SDL_BUTTON_LEFT && game->timers.timerTick >= 60) {
					if (game->SDLevent.motion.x >= 229 && game->SDLevent.motion.x <= 648 && game->SDLevent.motion.y >= 132 && game->SDLevent.motion.y <= 217) {
						game->state = START_SEQUENCE;
						first_frame = false;
						break;
					}
					else if (game->SDLevent.motion.x >= 229 && game->SDLevent.motion.x <= 435 && game->SDLevent.motion.y >= 413 && game->SDLevent.motion.y <= 502) {
						game->state = COMP;
						break;
					}
					else if (game->SDLevent.motion.x >= 448 && game->SDLevent.motion.x <= 648 && game->SDLevent.motion.y >= 413 && game->SDLevent.motion.y <= 502) {
						game->state = OPTIONSMENU;
						break;
					}
				}
				handle_menu_frame(game, &game->bmp.menu);
			}
			break;
		}
		case OPTIONSMENU: {
			if (game->event == TIMER) {
				handle_menu_frame(game, &game->bmp.options);
			}
			if (game->event == MOUSE) {
				if (game->SDLevent.button.button == SDL_BUTTON_LEFT) {
					if (game->SDLevent.motion.y >= 74 && game->SDLevent.motion.y <= 98) {
						if (game->SDLevent.motion.x >= 547 && game->SDLevent.motion.x <= 567)
							game->settings.fps = 2;
						else if (game->SDLevent.motion.x >= 660 && game->SDLevent.motion.x <= 680)
							game->settings.fps = 1;
						else if (game->SDLevent.motion.x >= 870 && game->SDLevent.motion.x <= 890)
							game->settings.fps = 0;
					}
					if (game->SDLevent.motion.y >= 99 && game->SDLevent.motion.y <= 120) {
						if (game->SDLevent.motion.x >= 583 && game->SDLevent.motion.x <= 603)
							game->settings.m_sens = 0.5;
						else if (game->SDLevent.motion.x >= 712 && game->SDLevent.motion.x <= 732)
							game->settings.m_sens = 1;
						else if (game->SDLevent.motion.x >= 838 && game->SDLevent.motion.x <= 858)
							game->settings.m_sens = 2;
					}
					if (game->SDLevent.motion.y >= 173 && game->SDLevent.motion.y <= 193 && game->SDLevent.motion.x >= 368 && game->SDLevent.motion.x <= 389)
						game->settings.fps_counter ^= 1;

					if (game->SDLevent.motion.y >= 704 && game->SDLevent.motion.y <= 756 && game->SDLevent.motion.x >= 13 && game->SDLevent.motion.x <= 125)
						game->state = MENU;
				}
			}
			break;
		}
		
		case START_SEQUENCE: {
			if (game->event == TIMER) {
				
				if (game->timers.start_seq == 3) {
					game->timers.timerTick = 0;
					ship_spawn(&game->player1);
					if (game->alien.active)
						game->alien.active = false;
				}

				if (game->timers.start_seq == 0 && (game->timers.timerTick % 60 == 0)) {
					game->s_event = IDLING;
					start_timers(&game->timers);
					game->state = NEW_ROUND;
					break;
				}

				if (game->timers.timerTick % 60 == 0) {
					render_seq_frame(game);
					display_frame();
					game->timers.start_seq--;
				}
			}

			break;
		}
		case COMP: {return;}
		default: break;
	}
}

