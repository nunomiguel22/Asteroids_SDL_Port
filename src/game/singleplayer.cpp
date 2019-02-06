#include "game.h"
#include "renderer.h"
#include "alien.h"

void singleplayer_event_handler(game_data* game) {
	switch (game->event) {

	case KEYBOARD: {

		/* Toggle console */
		if (game->SDLevent.key.keysym.sym == SDLK_BACKSLASH || game->SDLevent.key.keysym.sym == SDLK_BACKQUOTE) {
			game->console.toggle();
			game->console.write("The game is paused, to continue close the console and press ESC", C_NORMAL);
			game->state = SP_GAMEPAUSED;
		}
		/* Pause on escape */
		if (game->player1.s_event == K_ESC) 
			game->state = SP_GAMEPAUSED;
		
		/* Apply force to ship */
		else if (ship_apply_force(&game->player1.s_event, &game->player1))
			game->threads.push(play_sound, std::ref(*game->sound.thrust));

		break;
	}

	case MOUSE: {
		game->threads.push(mouse_handler, std::ref(*game));
		break;
	}

	case TIMER: {
		/* Physics update */
		game->threads.push(physics_update, std::ref(*game));
		/* Locked fps render */
		if (game->settings.fps)
			if (game->timers.timerTick % game->settings.fps == 0)
				handle_frame(game);
		/* Fire rate controller */
		if (!(game->player1.status & BIT(7)) && game->timers.player1_weapon_timer >= FIRE_RATE)
			game->player1.status |= BIT(7);
		/* Jump rate controller */
		if (!(game->player1.status & BIT(6)) && game->timers.teleport_timer >= (JUMP_RATE * 60))
			game->player1.status |= BIT(6);

		break;
	}
	default: break;
	}
}

void singleplayer_start_sequence(game_data *game) {
	
	if (game->event == TIMER) {

		/* First second of sequence iniate player ship, reset alien ship*/
		if (game->timers.start_seq == 3) {
			if (game->settings.music_volume)
				game->threads.push(play_music, std::ref(*game->sound.kawaii));
			game->timers.timerTick = 0;
			ship_spawn(&game->player1);
			if (game->alien.status & BIT(0))
				game->alien.status &= ~BIT(0);
		}

		/* Last second of sequence initiate game timers, start game*/
		if (game->timers.start_seq == 0 && (game->timers.timerTick % 60 == 0)) {
			game->threads.push(play_sound, std::ref(*game->sound.cdownb));
			game->player1.s_event = IDLING;
			start_timers(&game->timers);
			game->state = SP_NEW_ROUND;
			return;
		}

		/* Update screen 60 times a second */
		if (game->timers.timerTick % 60 == 0) {
			game->threads.push(play_sound, std::ref(*game->sound.cdowna));
			render_seq_frame(game);
			display_frame();
			game->timers.start_seq--;
		}
	}
}

void singleplayer_game_over(game_data *game) {
	
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
				game->state = SP_START_SEQUENCE;
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
}

void singleplayer_pause(game_data *game) {

	game->timers.start_seq = 3;
	
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
			return;
		}

		if (game->player1.s_event == K_ESC)
			game->state = SINGLEPLAYER;
		else if (game->player1.s_event == QUIT) {
			game->threads.push(stop_music);
			game->state = MENU;
		}
	}
	if (game->event == TIMER)
		if (game->settings.fps && game->timers.timerTick % game->settings.fps == 0)
				handle_frame(game);
}




void mouse_handler(int id, game_data& game) {

	/* Update the crosshair's position */
	game.player1.crosshair.x = (float)(game.SDLevent.motion.x - hres_center);
	game.player1.crosshair.y = (float)(vres_center - game.SDLevent.motion.y);

	/* Fire lasers */
	if (game.SDLevent.button.button == SDL_BUTTON_LEFT && (game.player1.status & BIT(7))) {
		ship_fire_laser(&game.player1, &game.timers.player1_weapon_timer);
		game.threads.push(play_sound, std::ref(*game.sound.laser));
	}

	/* Engage teleport */
	if (game.SDLevent.button.button == SDL_BUTTON_RIGHT && (game.player1.status & BIT(6))) {
		game.threads.push(play_sound, std::ref(*game.sound.teleport));
		game.player1.status |= BIT(5);
		game.player1.teleport_time = game.timers.timerTick + 20;
	}
}

void new_round_reset(game_data* game) {

	if (game->player1.round < MAX_ASTEROIDS)
		game->player1.round += ASTEROID_INCREASE_RATE;
	if (game->player1.hp < PLAYER_MAX_HEALTH)
		game->player1.hp += PLAYER_HEALTH_REGENERATION;
	if (game->player1.hp > PLAYER_MAX_HEALTH)
		game->player1.hp = PLAYER_MAX_HEALTH;

	ast_spawn(game->asteroid_field, &game->player1);
	int random_alien_spawn = rand() % (100 - 1) + 1;
	if (random_alien_spawn > (100 - ((game->player1.round - STARTING_ASTEROIDS) * ALIEN_SPAWN_CHANCE_INCREASE))) {
		game->threads.push(play_sound, std::ref(*game->sound.alien_spawn));
		alien_spawn(&game->alien);
	}

	game->timers.round_timer = 0;
	game->player1.status |= BIT(3);
	game->player1.status &= ~BIT(4);
	game->state = SINGLEPLAYER;
}