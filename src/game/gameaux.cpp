#include <sstream>

#include "game.h"
#include "renderer.h"
#include "alien.h"


int game_data_init(game_data *game) {

	/* Set number of threads*/
	game->threads.resize(8);

	/* Load sound files */ //GAME CRASHING WHEN EFFECTS ON BUT MUSIC OFF : S
	if (load_sounds(&game->sound, game->settings.effects_volume, game->settings.music_volume, &game->console))
		return 1;

	/* Load font */
	std::string font_path;
	game->console.write_to_log("LOADING FONT FILES");

	font_path = FILE_FONTS_PATH"Lucida_Console.ttf";
	if (game->ttf_fonts.lucida_console_med.load(font_path, CON_FONT_SIZE)) {
		game->console.write_to_log("Couldn't load \"" + font_path + "\", error: " + SDL_GetError());
		return 1;
	}
	else game->console.write_to_log("Loaded \"" + font_path + "\"");

	font_path = FILE_FONTS_PATH"copperplategothicbold.ttf";
	if (game->ttf_fonts.copperplategothicbold.load(font_path, LARGE_FONT_SIZE)) {
		game->console.write_to_log("Couldn't load \"" + font_path + "\", error: " + SDL_GetError());
		return 1;
	}
	else game->console.write_to_log("Loaded \"" + font_path + "\"");

	font_path = FILE_FONTS_PATH"copperplategothicbold.ttf";
	if (game->ttf_fonts.copperplategothicbold_massive.load(font_path, MASSIVE_FONT_SIZE)) {
		game->console.write_to_log("Couldn't load \"" + font_path + "\", error: " + SDL_GetError());
		return 1;
	}
	else game->console.write_to_log("Loaded \"" + font_path + "\"");

	game->console.write_to_log("LOADED FONT FILES");
	game->console.write_to_log(" ");

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
	/* Resets all asteroid death timers */
	for (unsigned int i = 0; i < MAX_ASTEROIDS; i++) {
		game->asteroid_field[i].death_timer = 0;
		game->asteroid_field[i].death_frame = 0;
	}

	game->console.setvisibility(false);

	/* Load bitmaps into memory*/
	if (load_bitmaps(&game->bmp, &game->console))
		return 1;

	game->console.write_to_log("Game initialized successfully");
	game->console.write_to_log(" ");
	game->console.write_to_log(" ");
	game->console.write_to_log(" ");
	game->console.write_to_log(" ");

	game->console.write_welcome_message();

	return 0;
}

void physics_update(int id, game_data &game) {

	bool allenemiesdefeated = true;

	if ( game.player1.teleport_time > 0 && game.timers.timerTick >= game.player1.teleport_time ) {
		ship_teleport(&game.player1, &game.timers.teleport_timer);
		game.player1.teleport_time = 0;
		game.player1.teleporting = false;
	}

	/* Asteroid update and collision */
	if (ast_update(game.asteroid_field))
		allenemiesdefeated = false;
	if (ast_collision(game.asteroid_field, &game.player1, &game.alien)) {
		game.threads.push(play_sound, std::ref(*game.sound.pop));
		allenemiesdefeated = false;
	}	

	/* Player ship update */
	ship_update(&game.player1);

	/* Alien ship update and collision */
	if (game.alien.active) {
		allenemiesdefeated = false;
		if (alien_update(&game.alien, &game.player1, &game.timers))
			game.threads.push(play_sound, std::ref(*game.sound.alien_laser));
		alien_collision(&game.alien, &game.player1, &game.timers);
		if (!game.alien.active)
			game.threads.push(play_sound, std::ref(*game.sound.ship_expl));
	}

	if (game.player1.hit_reg) {
		game.timers.hitreg_timer = 30;
		game.player1.hit_reg = false;
	}
	if (allenemiesdefeated)
		game.player1.end_round = true;

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

void start_timers(game_timers *timers) {

	timers->framecounter = 1;
	timers->frames_per_second = 0;
	timers->cyclecounter = 0;
	timers->player1_weapon_timer = 0;
	timers->player2_weapon_timer = 0;
	timers->teleport_timer = 0;
	timers->round_timer = 0;
	timers->alien_death_timer = 0;
	timers->hitreg_timer = 0;
}

void increment_timers(game_timers *timers, asteroid asteroid_field[]) {

	for (unsigned int i = 0; i < MAX_ASTEROIDS; ++i) 
		if (!asteroid_field[i].active && asteroid_field[i].death_timer > 0) {
			--asteroid_field[i].death_timer;
			if (asteroid_field[i].death_timer % 2 == 0)
				++asteroid_field[i].death_frame;
		}

	++timers->timerTick;
	++timers->player1_weapon_timer;
	++timers->player2_weapon_timer;
	++timers->alien_weapon_timer;
	++timers->teleport_timer;
	++timers->round_timer;
	if (timers->hitreg_timer > 0)
		--timers->hitreg_timer;
	if (timers->alien_death_timer > 0)
		--timers->alien_death_timer;
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
	game->player1.invulnerability = true;
	game->player1.end_round = false;
	game->state = PLAYING;
}

void options_button_check(game_data* game) {

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

	/* Effects volume options*/
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
		read_game_settings(&game->settings);
		game->state = MENU;
	}
	/* Apply Button */
	if (game->SDLevent.motion.y >= 690 && game->SDLevent.motion.y <= 740 && game->SDLevent.motion.x >= 846 && game->SDLevent.motion.x <= 995) {
		change_volume(&game->sound, game->settings.effects_volume, game->settings.music_volume);
		reset_sdl(&game->settings);
		save_game_settings(&game->settings);
		game->state = MENU;
	}
	/* Reset Button */
	if (game->SDLevent.motion.y >= 690 && game->SDLevent.motion.y <= 740 && game->SDLevent.motion.x >= 235 && game->SDLevent.motion.x <= 718)
		reset_game_settings(&game->settings);
}

void exec_console_cmd(std::string cmd, game_data* game) {

	game->console.write("> " + cmd, C_WHITE);

	std::stringstream str;
	str << cmd;
	std::string command;
	getline(str, command, ' ');

	std::map <std::string, console_commands> *cmd_map = game->console.get_command_map();

	switch ((*cmd_map)[command]) {
	case con_not_defined: { game->console.write("No such command", C_ERROR); return; }
		
		/* Network commands */
		case con_init_server: {
			std::string portstr;
			getline(str, portstr);
			if (portstr.empty())
				game->console.write("Please enter a valid port (0-65535)", C_ERROR);

			uint16_t port;
			try { port = std::stoi(portstr); }
			catch (const std::invalid_argument&) { game->console.write("Please enter a valid port (0-65535)", C_ERROR); return; }
			
			game->connection.init_local(port, &game->console);
			break;
		}
		case con_init_client: {
			game->connection.init_local(DEFAULT_CLIENT_PORT, &game->console);
			break;
		}
		case con_connect: {
			if (!game->connection.initialized()) {
				game->console.write("Initiate network first ", C_ERROR);
				game->console.write("Use init_server portnumber or init_client", C_NORMAL);
				return;
			}

			std::string ip;
			std::string portstr;
			getline(str, ip, ':');
			getline(str, portstr);
			uint16_t port;
			try { port = std::stoi(portstr); } 
			catch (const std::invalid_argument&) { game->console.write("Invalid address format", C_ERROR); return; }

			if (ip.empty() || portstr.empty()) {
				game->console.write("Invalid address format", C_ERROR);
				return;
			}
			game->connection.connect(ip, port);
			break;
		}

		case con_send_testpacket: {
			std::string message;
			getline(str, message);
			if (message.empty()) {
				game->console.write("Select a message for the test packet", C_ERROR);
				game->console.write("Use send_testpacket message", C_NORMAL);
				return;
			}

			if (game->connection.connected() && game->connection.initialized()) {
				game->connection.send_packet(message);
				game->console.write("Packet sent", C_SUCCESS);
			}
			else {
				game->console.write("Initiate network and connect to a remote peer first", C_ERROR);
				game->console.write("Use init_server portnumber or init_client", C_NORMAL);
				game->console.write("Use connect IP:PORT to establish a connection", C_NORMAL);
			}

			break;
		}

		case con_receive_testpacket: {
			if (game->connection.connected() && game->connection.initialized()) {
				std::string value;
				value = game->connection.listen_packet();
				if (value == "NULL")
					game->console.write(value, C_NORMAL);
				else game->console.write("Received package with message: \"" + value + "\"", C_SUCCESS);
			}
			else {
				game->console.write("Initiate network and connect to a remote peer first", C_ERROR);
				game->console.write("Use init_server portnumber or init_client", C_NORMAL);
				game->console.write("Use connect IP:PORT to establish a connection", C_NORMAL);
			}

			break;
		}



		/* General commands */
		case con_quit: {
			game->state = COMP;
			break;
		}
		case con_clear: {
			game->console.clear_console();
			break;
		}
		default: return;
	}
}

void kill_sequence(game_data* game) {

	game->console.save_log_to_file();
	free_bitmaps(&game->bmp);
	free_sounds(&game->sound);
	exit_sdl(game);

}
