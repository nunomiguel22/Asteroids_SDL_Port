#include "game.h"
#include "SDL.h"
#include "vcard.h"
#include <time.h>
#include <SDL_mixer.h>

int game_data_init(game_data *game) {

	/* Set number of threads*/
	game->threads.resize(8);

	/* Load sound files */
	load_sounds(&game->sound, game->settings.music_volume, game->settings.effects_volume);

	/* Load highscores */
	if (!load_highscores(game->highscores))
		for (int i = 0; i < 5; i++)
			game->highscores[i] = 0;

	/* Initiate variables */
	game->timers.timerTick = 0;
	game->timers.start_seq = 3;
	game->state = MENU;
	game->timers.cyclecounter = 0;
	game->alien.active = false;

	/* Load pixmaps into memory*/
	load_xpms(&game->xpm);

	/* Load bitmaps into memory*/
	if (load_bitmaps(&game->bmp))
		return 1;

	return 0;
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


void physics_update(int id, game_data &game) {

	/* Asteroid update and collision */
	ast_update(game.asteroid_field);
	if (ast_collision(game.asteroid_field, &game.player1, &game.alien))
		game.threads.push(play_sound, std::ref(*game.sound.pop));

	/* Player ship update */
	ship_update(&game.player1);

	/* Alien ship update and collision */
	if (game.alien.active) {
		alien_update(&game.alien, &game.player1, &game.timers);
		alien_collision(&game.alien, &game.player1, &game.timers);
		if (!game.alien.active)
			game.threads.push(play_sound, std::ref(*game.sound.pop));
	}

	/* Make player ship invulnerable at the start of the round */
	if (game.timers.round_timer <= 30)
		game.player1.invulnerability = true;
	else game.player1.invulnerability = false;

	/* Stop game on 0 HP */
	if (game.player1.hp <= 0) {
		game.player1.hp = 0;
		game.state = LOSS;
	}
}

void handle_frame(game_data *game) {

	render_frame(game);
	display_frame();
	game->timers.framecounter++;
}

void handle_menu_frame(game_data *game, Bitmap *bckgrd) {

	bckgrd->draw(0, 0);

	switch (game->state) {
	case MENU: {
		for (int i = 0; i < MAX_ASTEROIDS; i++) {
			mpoint2d ws_ast = vector_translate_gfx(&game->menu_asteroid_field[i].position, 1024, 768);
			//Active asteroids
			if (game->menu_asteroid_field[i].active) {
				if (game->menu_asteroid_field[i].size == MEDIUM)
					draw_ast(&game->menu_asteroid_field[i], &game->bmp.medium_asteroid);
				else draw_ast(&game->menu_asteroid_field[i], &game->bmp.large_asteroid);
			}
		}

		game->bmp.hsbackground.draw(787, 1);
		game->bmp.playbutton.draw(380, 200);
		game->bmp.optionsbutton.draw(380, 300);
		game->bmp.quitbutton.draw(380, 400);
		for (int i = 0; i < 5; i++)
			draw_number(game->highscores[i], 900, 60 + i * 40, game);
		break;
	}
	case OPTIONSMENU: {
		/* FPS Counter */
		if (game->settings.fps_counter)
			game->bmp.boxticked.draw(348, 270);
		/* Vsync */
		if (game->settings.vsync)
			game->bmp.boxticked.draw(151, 268);
		/* Resolution */
		if (game->settings.hresolution == 1920)
			game->bmp.p_arrow.draw(55,150);
		else if (game->settings.hresolution == 1600)
			game->bmp.p_arrow.draw(62, 173);
		else if (game->settings.hresolution == 1280)
			game->bmp.p_arrow.draw(52, 196);
		else if (game->settings.hresolution == 1024)
			game->bmp.p_arrow.draw(60, 217);
		/* Display Mode */
		if (game->settings.fullscreen)
			game->bmp.p_arrow.draw(306, 147);
		else if(game->settings.fullscreennative)
			game->bmp.p_arrow.draw(222, 171);
		else if (game->settings.borderless)
			game->bmp.p_arrow.draw(300, 196);
		else game->bmp.p_arrow.draw(295, 221);
		/* FPS */
		if (!game->settings.fps)
			game->bmp.p_arrow.draw(500, 147);
		else if (game->settings.fps == 1)
			game->bmp.p_arrow.draw(555, 168);
		else if (game->settings.fps == 2)
			game->bmp.p_arrow.draw(555, 193);
		/* Music Volume */
		switch (game->settings.music_volume) {
			case 0: {
				game->bmp.slidemarker.draw(205, 382);
				break;
			}
			case 1: {
				game->bmp.slidemarker.draw(232, 382);
				break;
			}
			case 2: {
				game->bmp.slidemarker.draw(258, 382);
				break;
			}
			case 3: {
				game->bmp.slidemarker.draw(285, 382);
				break;
			}
			case 4: {
				game->bmp.slidemarker.draw(312, 382);
				break;
			}
			case 5: {
				game->bmp.slidemarker.draw(339, 382);
				break;
			}
			default: break;
		}

		/* Effects Volume */
		switch (game->settings.effects_volume) {
		case 0: {
			game->bmp.slidemarker.draw(205, 454);
			break;
		}
		case 1: {
			game->bmp.slidemarker.draw(232, 454);
			break;
		}
		case 2: {
			game->bmp.slidemarker.draw(258, 454);
			break;
		}
		case 3: {
			game->bmp.slidemarker.draw(285, 454);
			break;
		}
		case 4: {
			game->bmp.slidemarker.draw(312, 454);
			break;
		}
		case 5: {
			game->bmp.slidemarker.draw(339, 454);
			break;
		}
		default: break;
		}

		break;
	}
	default: break;

	}

	game->xpm.cursor.draw(game->SDLevent.motion.x + 5, game->SDLevent.motion.y + 7, false);

	display_frame();
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
		case OPTIONSMENU: {
			static bool gfxchange = false;

			if (game->event == TIMER) 
				handle_menu_frame(game, &game->bmp.options);
			
			if (game->event == MOUSE) {
				if (game->SDLevent.button.button == SDL_BUTTON_LEFT) {

					/* Display mode options*/
					if (game->SDLevent.motion.x >= 215 && game->SDLevent.motion.x <= 410) {
						if (game->SDLevent.motion.y >= 147 && game->SDLevent.motion.y <= 167) {
							game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
							game->settings.fullscreen = true;
						}
						else if (game->SDLevent.motion.y >= 169 && game->SDLevent.motion.y <= 192) {
							game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
							game->settings.fullscreennative = true;
							game->settings.fullscreen = false;
						}
						else if (game->SDLevent.motion.y >= 194 && game->SDLevent.motion.y <= 215) {
							game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
							game->settings.fullscreennative = false;
							game->settings.fullscreen = false;
							game->settings.borderless = true;
						}
						else if (game->SDLevent.motion.y >= 217 && game->SDLevent.motion.y <= 240) {
							game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
							game->settings.fullscreennative = false;
							game->settings.fullscreen = false;
							game->settings.borderless = false;
						}
							
					}
					/* Resolution options*/
					if (game->SDLevent.motion.x >= 37 && game->SDLevent.motion.x <= 158) {
						if (game->SDLevent.motion.y >= 147 && game->SDLevent.motion.y <= 167) {
							game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
							game->settings.vresolution = 1080;
							game->settings.hresolution = 1920;
						}
						else if (game->SDLevent.motion.y >= 169 && game->SDLevent.motion.y <= 192) {
							game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
							game->settings.vresolution = 900;
							game->settings.hresolution = 1600;
						}
						else if (game->SDLevent.motion.y >= 194 && game->SDLevent.motion.y <= 215) {
							game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
							game->settings.vresolution = 1024;
							game->settings.hresolution = 1280;
						}
						else if (game->SDLevent.motion.y >= 217 && game->SDLevent.motion.y <= 240) {
							game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
							game->settings.vresolution = 768;
							game->settings.hresolution = 1024;
						}

					}
					/* FPS options*/
					if (game->SDLevent.motion.x >= 445 && game->SDLevent.motion.x <= 610) {
						if (game->SDLevent.motion.y >= 147 && game->SDLevent.motion.y <= 167) {
							game->settings.fps = 0;
							game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
						}
						else if (game->SDLevent.motion.y >= 169 && game->SDLevent.motion.y <= 192) {
							game->settings.fps = 1;
							game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
						}
						else if (game->SDLevent.motion.y >= 194 && game->SDLevent.motion.y <= 215) {
							game->settings.fps = 2;
							game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
						}
					}

					/* Music volume options*/
					if (game->SDLevent.motion.y >= 386 && game->SDLevent.motion.y <= 406) {
						if (game->SDLevent.motion.x >= 208 && game->SDLevent.motion.x <= 217)
							game->settings.music_volume = 0;
						else if (game->SDLevent.motion.x >= 236 && game->SDLevent.motion.x <= 245)
							game->settings.music_volume = 1;
						else if (game->SDLevent.motion.x >= 260 && game->SDLevent.motion.x <= 269)
							game->settings.music_volume = 2;
						else if (game->SDLevent.motion.x >= 287 && game->SDLevent.motion.x <= 296)
							game->settings.music_volume = 3;
						else if (game->SDLevent.motion.x >= 313 && game->SDLevent.motion.x <= 322)
							game->settings.music_volume = 4;
						else if (game->SDLevent.motion.x >= 340 && game->SDLevent.motion.x <= 349)
							game->settings.music_volume = 5;
					}

					/* Music volume options*/
					if (game->SDLevent.motion.y >= 458 && game->SDLevent.motion.y <= 478) {
						if (game->SDLevent.motion.x >= 208 && game->SDLevent.motion.x <= 217)
							game->settings.effects_volume = 0;
						else if (game->SDLevent.motion.x >= 236 && game->SDLevent.motion.x <= 245)
							game->settings.effects_volume = 1;
						else if (game->SDLevent.motion.x >= 260 && game->SDLevent.motion.x <= 269)
							game->settings.effects_volume = 2;
						else if (game->SDLevent.motion.x >= 287 && game->SDLevent.motion.x <= 296)
							game->settings.effects_volume = 3;
						else if (game->SDLevent.motion.x >= 313 && game->SDLevent.motion.x <= 322)
							game->settings.effects_volume = 4;
						else if (game->SDLevent.motion.x >= 340 && game->SDLevent.motion.x <= 349)
							game->settings.effects_volume = 5;
					}
				
					/* FPS counter option*/
					if (game->SDLevent.motion.y >= 270 && game->SDLevent.motion.y <= 290 && game->SDLevent.motion.x >= 345 && game->SDLevent.motion.x <= 370) {
						game->settings.fps_counter ^= 1;
						game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
					}
					/* Vsync option*/
					if (game->SDLevent.motion.y >= 270 && game->SDLevent.motion.y <= 290 && game->SDLevent.motion.x >= 147 && game->SDLevent.motion.x <= 174) {
						game->settings.vsync ^= 1;
						game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
					}
					/* Cancel Button */
					if (game->SDLevent.motion.y >= 690 && game->SDLevent.motion.y <= 740 && game->SDLevent.motion.x >= 38 && game->SDLevent.motion.x <= 185) {
						read_video_settings(&game->settings);
						game->state = MENU;
					}
					/* Apply Button */
					if (game->SDLevent.motion.y >= 690 && game->SDLevent.motion.y <= 740 && game->SDLevent.motion.x >= 846 && game->SDLevent.motion.x <= 995) {
						change_volume(&game->sound, game->settings.effects_volume, game->settings.music_volume);
						reset_sdl(&game->settings);
						save_video_settings(&game->settings);
						game->state = MENU;
					}
					/* Reset Button */
					if (game->SDLevent.motion.y >= 690 && game->SDLevent.motion.y <= 740 && game->SDLevent.motion.x >= 235 && game->SDLevent.motion.x <= 718) 
						reset_video_settings(&game->settings);
				}
			}
			break;
		}
		
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

