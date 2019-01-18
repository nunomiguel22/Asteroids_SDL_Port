#include <SDL.h>
#include <SDL_mixer.h>
#include <time.h>


#include "game.h"
#include "renderer.h"
#include "alien.h"


void event_handler(game_data* game) {

	double lastTick = SDL_GetTicks();
	clock_t previousRTC;
	previousRTC = clock();
	
	/* Exit program when state is complete */
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

		/* Count frames drawn every second */
		if ((clock() - previousRTC) / (double)CLOCKS_PER_SEC >= 1) {
			game->timers.frames_per_second = ++game->timers.framecounter;
			game->timers.framecounter = 0;
			previousRTC = clock();
		}
		/* Draw frames whenever possible if fps is in unlocked mode */
		if (!game->settings.fps && game->state == PLAYING)
			handle_frame(game);
	}
}

void game_state_machine(game_data* game) {

	switch (game->state) {

		/* MAIN MENU */
		case MENU: {
			static bool first_frame = false;
			/* Operations when entering menu: start music, initiate menu asteroids, etc */
			if (!first_frame) {
				game->threads.push(play_music, std::ref(*game->sound.galaxia));
				first_frame = true;
				game->alien.round = 7;
				ast_spawn(game->menu_asteroid_field, &game->alien);
				handle_menu_frame(game, &game->bmp.menubackground);
				game->timers.timerTick = 0;
			}

			if (game->event == MOUSE) 
				//Check menu selection
				if (game->SDLevent.button.button == SDL_BUTTON_LEFT && game->timers.timerTick >= 30	&& game->SDLevent.motion.x >= 380 && game->SDLevent.motion.x <= 570) {
					if (game->SDLevent.motion.y >= 207 && game->SDLevent.motion.y <= 268) {
						game->threads.push(stop_music);
						game->state = START_SEQUENCE;
						first_frame = false;
						break;
					}
					else if (game->SDLevent.motion.y >= 407 && game->SDLevent.motion.y <= 467) {
						game->state = COMP;
						break;
					}
					else if (game->SDLevent.motion.y >= 308 && game->SDLevent.motion.y <= 366) {
						game->state = OPTIONSMENU;
						break;
					}
				}
			
			if (game->event == TIMER) {
				ast_update(game->menu_asteroid_field);
				handle_menu_frame(game, &game->bmp.menubackground);
			}
			break;
		}
		/* OPTIONS MENU */
		case OPTIONSMENU: {

			if (game->event == TIMER) 
				handle_menu_frame(game, &game->bmp.options);
			
			if (game->event == MOUSE) {
				if (game->SDLevent.button.button == SDL_BUTTON_LEFT) {
					options_button_check(game);
				}
			}
			break;
		}
		
		/* COUNTDOWN TO SINGLE PLAYER GAME */
		case START_SEQUENCE: {
			if (game->event == TIMER) {

				/* First second of sequence iniate player ship, reset alien ship*/
				if (game->timers.start_seq == 3) {
					game->threads.push(play_music, std::ref(*game->sound.kawaii));
					game->timers.timerTick = 0;
					ship_spawn(&game->player1);
					if (game->alien.active)
						game->alien.active = false;
				}

				/* Last second of sequence initiate game timers, start game*/
				if (game->timers.start_seq == 0 && (game->timers.timerTick % 60 == 0)) {
					game->s_event = IDLING;
					start_timers(&game->timers);
					game->state = NEW_ROUND;
					break;
				}

				/* Update screen 60 times a second */
				if (game->timers.timerTick % 60 == 0) {
					render_seq_frame(game);
					display_frame();
					game->timers.start_seq--;
				}
			}
			break;
		}
		/* CURRENTLY PLAYING */
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
		/* NEW ROUND EVERY TIME ALL ENEMIES DIE */
		case NEW_ROUND: { 
			new_round_reset(game); 
			break;
		}

		/* WHEN PLAYER HP DROPS BELOW 1*/
		case LOSS: {
			game->threads.push(stop_music);
			static bool highscore;
			static bool first_frame = true;
			if (first_frame) {
				game->threads.push(stop_music);
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
		/* PAUSE STATE*/
		case GAMEPAUSED: {
			game->timers.start_seq = 3;
			if (game->event == KEYBOARD) {
				if (game->s_event == K_ESC)
					game->state = PLAYING;
				else if (game->s_event == QUIT) {
					game->threads.push(stop_music);
					game->state = MENU;
				}
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
			game->player1.crosshair.x = (game->SDLevent.motion.x - hres / 2);
			game->player1.crosshair.y = (vres / 2 - game->SDLevent.motion.y);

			/* Fire lasers */
			if (game->SDLevent.button.button == SDL_BUTTON_LEFT && game->player1.weapon_ready) {
				ship_fire_laser(&game->player1, &game->timers.player1_weapon_timer);
				game->threads.push(play_sound, std::ref(*game->sound.laser));
			}

			/* Teleport */
			if (game->SDLevent.button.button == SDL_BUTTON_RIGHT && game->player1.jump_ready)
				ship_teleport(&game->player1, &game->timers.teleport_timer);

			break;
		}

		case TIMER: {
			/* Physics update */
			if (game->timers.timerTick % PHYSICS_TICKS == 0)
				game->threads.push(physics_update, std::ref(*game));
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

