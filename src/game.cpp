#include "game.h"
#include "SDL.h"
#include "vcard.h"
#include <time.h>

void physics_update(game_data *game) {

	ast_update(game->asteroid_field);
	ast_collision(game->asteroid_field, &game->player1, &game->alien);
	ship_update(&game->player1);
	if (game->alien.active) {
		alien_update(&game->alien, &game->player1, &game->timers);
		alien_collision(&game->alien, &game->player1, &game->timers);
	}
	if (game->timers.round_timer <= 30)
		game->player1.invulnerability = true;
	else game->player1.invulnerability = false;

	if (game->player1.hp <= 0) {
		game->player1.hp = 0;
		game->state = LOSS;
	}
}

void handle_frame(game_data * game) {

	render_frame(game);
	display_frame();
	game->timers.framecounter += 1;
}

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

void increment_timers(game_timers *timers) {
	timers->timerTick++;
	timers->player1_weapon_timer++;
	timers->player2_weapon_timer++;
	timers->alien_weapon_timer++;
	timers->teleport_timer++;
	timers->round_timer++;
	if (timers->alien_death_timer > 0)
		timers->alien_death_timer--;

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
	game->alien.active = false;

	load_xpms(&game->xpm);
	if (load_bitmaps(&game->bmp))
		return 1;

	return 0;
}

void event_handler(game_data* game) {

	double lastTick = SDL_GetTicks();

	clock_t previousRTC;
	previousRTC = clock();
	
	while (game->state != COMP) {

		double currentTick = SDL_GetTicks();
		
		/* Input event from keyboard or mouse */
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

					switch (game->SDLevent.key.keysym.sym) {
						case SDLK_w:	game->s_event = MAIN_THRUSTER;	break;
						case SDLK_a:	game->s_event = PORT_THRUSTER;	break;
						case SDLK_d:	game->s_event = STARBOARD_THRUSTER;	break;
						case SDLK_s:	game->s_event = REVERSE_THRUSTER;	break;
						case SDLK_ESCAPE:	game->s_event = K_ESC;	break;
						case SDLK_SPACE:	game->s_event = QUIT; break;

						default: game->s_event = IDLING;
					}

					game->event = KEYBOARD;
					game_state_machine(game);


					break;
				}
				case SDL_KEYUP: {
					game->s_event = IDLING;
					break;
				}

			}

		}

		/* Timer event, 60 times a second */
		if (currentTick - lastTick >= 17) {

			increment_timers(&game->timers);
			game->event = TIMER;
			game_state_machine(game);
			lastTick = currentTick;
		}

		if ((clock() - previousRTC) / (double)CLOCKS_PER_SEC >= 1) {
			game->timers.frames_per_second = ++game->timers.framecounter;
			game->timers.framecounter = 0;
			previousRTC = clock();
		}
		if (!game->settings.fps && game->state == PLAYING)
			handle_frame(game);

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
					if (game->SDLevent.motion.x >= 244 && game->SDLevent.motion.x <= 536 && game->SDLevent.motion.y >= 180 && game->SDLevent.motion.y <= 240) {
						game->state = START_SEQUENCE;
						first_frame = false;
						break;
					}
					else if (game->SDLevent.motion.x >= 244 && game->SDLevent.motion.x <= 389 && game->SDLevent.motion.y >= 247 && game->SDLevent.motion.y <= 306) {
						game->state = COMP;
						break;
					}
					else if (game->SDLevent.motion.x >= 394 && game->SDLevent.motion.x <= 536 && game->SDLevent.motion.y >= 247 && game->SDLevent.motion.y <= 306) {
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

		case PLAYING: {
			static int round_delay = 0;
			playing_event_handler(game);

			if (game->player1.end_round) {
				if (game->event == TIMER)
					round_delay++;

				if (round_delay >= DELAY_BETWEEN_ROUNDS) {
					round_delay = 0;
					game->state = NEW_ROUND;
				}
			}
			break;
		}
		case NEW_ROUND: {

			if (game->player1.round < MAX_ASTEROIDS)
				game->player1.round += ASTEROID_INCREASE_RATE;
			if (game->player1.hp < PLAYER_MAX_HEALTH)
				game->player1.hp += PLAYER_HEALTH_REGENERATION;
			if (game->player1.hp > PLAYER_MAX_HEALTH)
				game->player1.hp = PLAYER_MAX_HEALTH;

			ast_spawn(game->asteroid_field, &game->player1);
			int random_alien_spawn = rand() % (100 - 1) + 1;
			if (random_alien_spawn >(100 - ((game->player1.round - STARTING_ASTEROIDS) * ALIEN_SPAWN_CHANCE_INCREASE)))
				alien_spawn(&game->alien);

			game->timers.round_timer = 0;
			game->player1.invulnerability = true;
			game->state = PLAYING;

			break;
		}
		case LOSS: {
			static bool highscore;
			static bool first_frame = true;
			if (first_frame) {
				game->timers.start_seq = 3;
				first_frame = false;
				highscore = verify_highscores(game->highscores, &game->player1);
			}
			switch (game->event) {

				case MOUSE: {

					if (game->SDLevent.button.button == SDL_BUTTON_LEFT) {
						if (game->SDLevent.motion.x >= 227 && game->SDLevent.motion.x <= 444 && game->SDLevent.motion.y >= 385 && game->SDLevent.motion.y <= 454) {
							game->state = START_SEQUENCE;
							first_frame = true;
						break;
						}
						else if (game->SDLevent.motion.x >= 521 && game->SDLevent.motion.x <= 738 && game->SDLevent.motion.y >= 385 && game->SDLevent.motion.y <= 454) {
							game->state = MENU;
							first_frame = true;
						}
					}
					break;
				}

					case TIMER: {
					render_frame(game);
					if (highscore)
						handle_menu_frame(game, &game->bmp.death_screen_highscore);
					else handle_menu_frame(game, &game->bmp.death_screen);
					break;
				}
				default: break;
			}
			break;
		}

		case GAMEPAUSED: {
			game->timers.start_seq = 3;
			if (game->event == KEYBOARD) {
				if (game->s_event == K_ESC)
					game->state = PLAYING;
				else if (game->s_event == QUIT)
					game->state = MENU;
			}
			break;
		}

		case COMP: {return;}
		default: break;
	}
}

void playing_event_handler(game_data* game) {
	switch (game->event) {

		case KEYBOARD: {
			/* Pause on escape */
			if (game->s_event == K_ESC) {
				render_frame(game);
				game->bmp.pause_message.draw(0, 0);
				display_frame();
				game->state = GAMEPAUSED;
			}
			/* Apply force to ship */
			else ship_apply_force(&game->s_event, &game->player1);
			break;
		}

		case MOUSE: {
			/* Update the crosshair's position */
			game->player1.crosshair.x = (game->SDLevent.motion.x - hres / 2) * game->settings.m_sens;
			game->player1.crosshair.y = (vres / 2 - game->SDLevent.motion.y) * game->settings.m_sens;

			/* Fire lasers */
			if (game->SDLevent.button.button == SDL_BUTTON_LEFT && game->player1.weapon_ready)
				ship_fire_laser(&game->player1, &game->timers.player1_weapon_timer);

			/* Teleport */
			if (game->SDLevent.button.button == SDL_BUTTON_RIGHT && game->player1.jump_ready)
				ship_teleport(&game->player1, &game->timers.teleport_timer);

			break;
		}

		case TIMER: {
			/* Physics update */
			if (game->timers.timerTick % PHYSICS_TICKS == 0)
				physics_update(game);
			/* Locked fps render */
			if (game->settings.fps)
				if (game->timers.timerTick % game->settings.fps == 0)
					handle_frame(game);
			/* Fire rate controller */
			if (game->timers.player1_weapon_timer >= (60 / FIRE_RATE) && !(game->player1.weapon_ready))
				game->player1.weapon_ready = true;
			/* Jump rate controller */
			if (game->timers.teleport_timer >= (JUMP_RATE * 60) && !(game->player1.jump_ready))
				game->player1.jump_ready = true;

			break;
		}
		default: break;
	}
}

