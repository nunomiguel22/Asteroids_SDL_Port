#include <sstream>

#include "game.h"
#include "../renderer/renderer.h"

//#include <iostream> //DEBUG
void start_listening(int id, game_data &game) {

	unsigned int current_tick = 0;
	std::stringstream cvt;
	net_message msg;
	game.player2.status &= ~BIT(0);
	while (game.connection.is_connected() || game.connection.server_is_open()) {

		game.connection.listen_packet(msg);

		 if (msg.header == "game") {
				if (msg.timer_tick > current_tick) {
					game.player2 = msg.playern;
					current_tick = msg.timer_tick;
				}
		}
		 else if (msg.header == "collision") {
			 game.player1.force = msg.playern.force;
			 game.player1.hp = msg.playern.hp;
			 for (int i = 0; i < AMMO; ++i)
				game.player1.lasers[i] = msg.playern.lasers[i];
		 }

		else if (msg.header == "info packet") {
			game.console.write("Info packet received", C_SUCCESS);

			game.console.write("Packet header: \"" + msg.header + "\"", C_NORMAL);

			cvt << game.timers.timerTick;
			game.console.write("Local timer tick: \"" + cvt.str() + "\"", C_NORMAL);
			cvt.clear();

			cvt << msg.timer_tick;
			game.console.write("Remote timer tick: \"" + cvt.str() + "\"", C_NORMAL);
			cvt.clear();

			game.console.write(" ", C_NORMAL);
		}

		else if (msg.header == "client") {
			std::stringstream str;
			str << msg.ip << ":" << msg.port;

			game.connection.resolve_ip(msg.ip, msg.port);
			game.player2 = msg.playern;
			game.threads.push(start_transmitting, std::ref(game));

			game.console.write("Client connected, IP: " + str.str(), C_SUCCESS);
		}
	}
}

void start_transmitting(int id, game_data &game) {

	int tick_next = 0;
	while (game.connection.is_connected() || game.connection.server_is_open()) {

		if (game.event == TIMER) {
			net_message game_info;
			game_info.playern = game.player1;
			game_info.header = "game";
			game_info.timer_tick = game.timers.timerTick;
			game.connection.send_packet(game_info);
		}
	}
}


void multiplayer_event_handler(game_data* game) {
	switch (game->event) {

	case KEYBOARD: {
		/* Toggle console */
		if (game->SDLevent.key.keysym.sym == SDLK_BACKSLASH || game->SDLevent.key.keysym.sym == SDLK_BACKQUOTE)
			game->console.toggle();
		/* Console input */
		if (game->console.is_open()) {
			std::string cmd;
			cmd = game->console.input_handler(game->SDLevent);
			if (!cmd.empty())
				exec_console_cmd(cmd, game);
			break;
		}
		/* Apply force to ship */
		if (ship_apply_force(&game->player1.s_event, &game->player1))
			game->threads.push(play_sound, std::ref(*game->sound.thrust));

		break;
	}

	case MOUSE: {
		game->threads.push(mouse_handler, std::ref(*game));
		break;
	}

	case TIMER: {
		/* Physics update */
		game->threads.push(physics_mp_update, std::ref(*game));
		/* Locked fps render */
		if (game->settings.fps)
			if (game->timers.timerTick % game->settings.fps == 0)
				handle_mp_frame(game);
		/* Fire rate controller */
		if (!(game->player1.status & BIT(7)) && game->timers.player1_weapon_timer >= FIRE_RATE)
			game->player1.status |= BIT(7);
		/* Jump rate controller */
		if (game->timers.teleport_timer >= (JUMP_RATE * 60) && !(game->player1.status & BIT(6)))
			game->player1.status |= BIT(6);

		break;
	}
	default: break;
	}
}


void physics_mp_update(int id, game_data &game) {

	if (game.player1.teleport_time > 0 && game.timers.timerTick >= game.player1.teleport_time) {
		ship_teleport(&game.player1, &game.timers.teleport_timer);
		game.player1.teleport_time = 0;
		game.player1.status &= ~BIT(5);
	}

	/* Player ship update */
	ship_update(&game.player1);
	if (game.player2.status & BIT(0))
		ship_update(&game.player2);

	if (game.host && (game.player2.status & BIT(0)))
		if (ship_mp_collision(&game.player1, &game.player2)) {

			net_message col_info;
			col_info.playern = game.player2;
			col_info.header = "collision";
			col_info.timer_tick = game.timers.timerTick;
			game.connection.send_packet(col_info);
		}

	if (game.player1.status & BIT(2)) {
		game.timers.hitreg_timer = 30;
		game.player1.status &= ~BIT(2);
	}
}

void multiplayer_start_sequence(game_data *game) {

	if (game->event == TIMER) {

		/* First second of sequence iniate player ship, reset alien ship*/
		if (game->timers.start_seq == 3) {
			if (game->settings.music_volume)
				game->threads.push(play_music, std::ref(*game->sound.kawaii));
			ship_mp_reset(&game->player1, &game->player2, game->host);
		}

		/* Last second of sequence initiate game timers, start game*/
		if (game->timers.start_seq == 0 && (game->timers.timerTick % 60 == 0)) {
			game->threads.push(play_sound, std::ref(*game->sound.cdownb));
			game->timers.start_seq = 3;
			game->player1.s_event = IDLING;
			game->state = MP_ROUND;
			return;
		}

		/* Update screen 60 times a second */
		if (game->timers.timerTick % 60 == 0) {
			game->threads.push(play_sound, std::ref(*game->sound.cdowna));
			render_mp_seq_frame(game);
			display_frame();
			game->timers.start_seq--;
		}
	}
}