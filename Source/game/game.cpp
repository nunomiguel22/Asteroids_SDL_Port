#include <time.h>


#include "game.h"
#include "../renderer/renderer.h"
#include "../entities/alien.h"

#include <iostream> //DEGUB

void event_handler(game_data* game) {

	uint32_t lastTick = SDL_GetTicks();
	clock_t previousRTC = clock();
	
	/* Exit program when state is complete */
	while (game->state != COMP) {

		uint32_t currentTick = SDL_GetTicks();
		
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
						case SDLK_w:	game->player1.s_event = MAIN_THRUSTER;	break;
						case SDLK_a:	game->player1.s_event = PORT_THRUSTER;	break;
						case SDLK_d:	game->player1.s_event = STARBOARD_THRUSTER;	break;
						case SDLK_s:	game->player1.s_event = REVERSE_THRUSTER;	break;
						case SDLK_ESCAPE:	game->player1.s_event = K_ESC;	break;
						case SDLK_SPACE:	game->player1.s_event = QUIT; break;
						case SDLK_F3: {
							if (game->state == MP_WARMUP) {
								if (game->player1.status & BIT(1))
									game->player1.status &= ~BIT(1);									
								else game->player1.status |= BIT(1);
							}
						}

						default: game->player1.s_event = IDLING;
					}
					game->event = KEYBOARD;
					game_state_machine(game);
					game->event = EV_IDLE;
					break;
				}
				case SDL_KEYUP: {
					game->player1.s_event = IDLING;
					break;
				}
				case SDL_TEXTINPUT: {
					game->event = KEYBOARD;
					game_state_machine(game);
					game->event = EV_IDLE;
					break;
				}
			
			}
		}

		/* Timer event, 60 times a second */
		if (currentTick - lastTick >= 17) {
			increment_timers(&game->timers, game->asteroid_field);
			game->event = TIMER;
			game_state_machine(game);
			lastTick = currentTick;
			game->event = EV_IDLE;
		}

		/* Count frames drawn every second */
		if ((clock() - previousRTC) / (float)CLOCKS_PER_SEC >= 1) {
			game->timers.frames_per_second = ++game->timers.framecounter;
			game->timers.framecounter = 0;
			previousRTC = clock();
		}
		/* Draw frames whenever possible if fps is in unlocked mode */
		if (!game->settings.fps) {
			if (game->state == SINGLEPLAYER || game->state == SP_GAMEPAUSED)
				handle_frame(game);
			else if (game->state == MP_WARMUP || game->state == MP_ROUND)
				handle_mp_frame(game);
		}
	}
}

void game_state_machine(game_data* game) {

	switch (game->state) {

		case MENU: { main_menu_handler(game); break; }

		case OPTIONSMENU: { options_menu_handler(game); break; }
		
		case SP_START_SEQUENCE: { singleplayer_start_sequence(game); break; }
		
		case SINGLEPLAYER: {
			static int round_delay = 0;
			singleplayer_event_handler(game);

			if (game->player1.status & BIT(4)) {
				if (game->event == TIMER)
					round_delay++;

				if (round_delay >= DELAY_BETWEEN_ROUNDS) {
					round_delay = 0;
					game->state = SP_NEW_ROUND;
				}
			}
			break;
		}
		
		case SP_NEW_ROUND: { new_round_reset(game);	break; }

		case SP_LOSS: { singleplayer_game_over(game); break; }

		case SP_GAMEPAUSED: { singleplayer_pause(game); break; }

		case MP_WARMUP: {
			multiplayer_event_handler(game); 
			if ((game->player1.status & BIT(1)) && (game->player2.status & BIT(1))) {
				net_message game_info;
				game_info.playern = game->player1;
				game_info.header = "game";
				game_info.timer_tick = game->timers.timerTick;
				game->connection.send_packet(game_info);
				game->player2.mp_round = 0;
				game->player1.mp_round = 0;
				game->state = MP_START_SEQUENCE;

			}
			
			break; 
		}

		case MP_START_SEQUENCE: {multiplayer_start_sequence(game); break; }

		case MP_ROUND: { 
			static unsigned int delay = 0;
			multiplayer_event_handler(game); 


			if (delay && game->timers.timerTick >= delay) {
				delay = 0;
				game->state = MP_START_SEQUENCE;
				if (game->player1.mp_round > 7 || game->player2.mp_round > 7) {
					ship_mp_spawn(&game->player1, &game->player2, game->host);
					game->player1.status &= ~BIT(1);
					game->player2.status &= ~BIT(1);
					game->player2.hp = 100;
					game->player1.hp = 100;
					game->state = MP_WARMUP;
					break;
				}
			}

			if (!delay && game->player2.hp <= 0) {
				game->threads.push(play_sound, std::ref(*game->sound.ship_expl));
				game->timers.player2_death_timer = (unsigned int)(ALIEN_DEATH_DURATION * 60);
				
				++game->player1.mp_round;
				delay = game->timers.timerTick + 60;

				break;
			}

			if (!delay && game->player1.hp <= 0) {
				game->threads.push(play_sound, std::ref(*game->sound.ship_expl));
				game->timers.player1_death_timer = (unsigned int)(ALIEN_DEATH_DURATION * 60);
				delay = game->timers.timerTick + 60;
				game->player1.status &= ~BIT(0);
				break;
			}
		
			break; 
		}

		case COMP: {return;}

		default: break;
	}
}

