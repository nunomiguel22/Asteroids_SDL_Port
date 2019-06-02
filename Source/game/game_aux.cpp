#include <sstream>

#include "../entities/alien.h"
#include "../entities/ship.h"
#include "../general/netcode.h"
#include "../renderer/renderer.h"
#include "game.h"

int game_data_init(game_data* game) {
  /* Set number of threads*/
  game->threads.resize(8);

  /* Load sound files */  // GAME CRASHING WHEN EFFECTS ON BUT MUSIC OFF : S
  if (load_sounds(&game->sound, game->settings.effects_volume,
                  game->settings.music_volume, &game->console))
    return 1;

  /* Load font */
  std::string font_path;
  game->console.log_push("LOADING FONT FILES");

  font_path = FILE_FONTS_PATH "Lucida_Console.ttf";
  if (game->ttf_fonts.lucida_console_med.load(font_path, CON_FONT_SIZE)) {
    game->console.log_push("Couldn't load \"" + font_path +
                           "\", error: " + SDL_GetError());
    return 1;
  } else
    game->console.log_push("Loaded \"" + font_path + "\"");

  font_path = FILE_FONTS_PATH "copperplategothicbold.ttf";
  if (game->ttf_fonts.copperplategothicbold.load(font_path, LARGE_FONT_SIZE)) {
    game->console.log_push("Couldn't load \"" + font_path +
                           "\", error: " + SDL_GetError());
    return 1;
  } else
    game->console.log_push("Loaded \"" + font_path + "\"");

  font_path = FILE_FONTS_PATH "copperplategothicbold.ttf";
  if (game->ttf_fonts.copperplategothicbold_massive.load(font_path,
                                                         MASSIVE_FONT_SIZE)) {
    game->console.log_push("Couldn't load \"" + font_path +
                           "\", error: " + SDL_GetError());
    return 1;
  } else
    game->console.log_push("Loaded \"" + font_path + "\"");

  game->console.log_push("LOADED FONT FILES");
  game->console.log_push(" ");

  /* Load highscores */
  if (!load_highscores(game->highscores))
    for (int i = 0; i < 5; i++) game->highscores[i] = 0;

  /* Initiate variables */
  start_timers(&game->timers);
  game->ping = 0;
  game->timers.timerTick = 0;
  game->timers.start_seq = 3;
  game->player1.mp_round = 0;
  game->player2.mp_round = 0;
  game->state = MENU;
  game->timers.cyclecounter = 0;
  game->alien.status &= ~BIT(0);
  game->player1.name = game->settings.name;
  /* Resets all asteroid death timers */
  for (unsigned int i = 0; i < MAX_ASTEROIDS; i++) {
    game->asteroid_field[i].death_timer = 0;
    game->asteroid_field[i].death_frame = 0;
  }

  /* Load bitmaps into memory*/
  if (load_bitmaps(&game->bmp, &game->console)) return 1;

  game->console.log_push("Game initialized successfully");
  game->console.log_push(" ");
  game->console.log_push(" ");
  game->console.log_push(" ");
  game->console.log_push(" ");

  game->console.welcome_message();

  return 0;
}

void kill_sequence(game_data* game) {
  game->console.log_save();
  // delete &game->console;
  // delete &game->connection;

  free_bitmaps(&game->bmp);
  free_sounds(&game->sound);
  exit_sdl();
}

void physics_update(int id, game_data& game) {
  bool allenemiesdefeated = true;

  if (game.player1.teleport_time > 0 &&
      game.timers.timerTick >= game.player1.teleport_time) {
    ship_teleport(&game.player1, &game.timers.teleport_timer);
    game.player1.teleport_time = 0;
    game.player1.status &= ~BIT(5);
  }

  /* Asteroid update and collision */
  if (ast_update(game.asteroid_field)) allenemiesdefeated = false;
  if (ast_collision(game.asteroid_field, &game.player1)) {
    game.threads.push(play_sound, std::ref(*game.sound.pop));
    allenemiesdefeated = false;
  }

  /* Player ship update */
  ship_update(&game.player1);

  /* Alien ship update and collision */
  if (game.alien.status & BIT(0)) {
    allenemiesdefeated = false;
    if (alien_update(&game.alien, &game.player1, &game.timers))
      game.threads.push(play_sound, std::ref(*game.sound.alien_laser));
    alien_collision(&game.alien, &game.player1, &game.timers);
    if (!(game.alien.status & BIT(0)))
      game.threads.push(play_sound, std::ref(*game.sound.ship_expl));
  }

  if (game.player1.status & BIT(2)) {
    game.timers.hitreg_timer = 30;
    game.player1.status &= ~BIT(2);
  }
  if (allenemiesdefeated) game.player1.status |= BIT(4);

  /* Make player ship invulnerable at the start of the round */
  if (game.timers.round_timer <= 30)
    game.player1.status |= BIT(3);
  else
    game.player1.status &= ~BIT(3);

  /* Stop game on 0 HP */
  if (game.player1.hp <= 0) {
    game.player1.hp = 0;
    game.state = SP_LOSS;
  }
}

void exec_console_cmd(std::string cmd, game_data* game) {
  game->console.write("> " + cmd, C_WHITE);

  std::stringstream str;
  str << cmd;
  std::string command;
  getline(str, command, ' ');

  switch (game->console.get_command(command)) {
    case con_not_defined: {
      game->console.write("No such command", C_ERROR);
      return;
    }

    /* Network commands */
    case con_init_server: {
      if (game->connection.port_is_open()) game->connection.port_close();

      std::string portstr;
      getline(str, portstr);
      if (portstr.empty())
        game->console.write("Please enter a valid port (0-65535)", C_ERROR);

      uint16_t port;
      try {
        port = std::stoi(portstr);
      } catch (const std::invalid_argument&) {
        game->console.write("Please enter a valid port (0-65535)", C_ERROR);
        return;
      }

      if (game->connection.port_open(port)) {
        game->console.write("Server mode initiated", C_SUCCESS);
        game->console.write("   Port: " + portstr, C_NORMAL);

        if (game->connection.fecth_public_ip())
          game->console.write(
              "   Public IP: " + game->connection.get_public_ip(), C_NORMAL);

        else
          game->console.write("   Public IP: Not connected to the internet",
                              C_WARNING);
      } else
        game->console.write("Failed to open port...", C_ERROR);

      game->connection.server_open();
      game->threads.push(start_listening, std::ref(*game));

      break;
    }

    case con_init_client: {
      if (game->connection.port_is_open()) {
        if (game->connection.server_is_open()) {
          game->console.write("Server mode detected, closing server...",
                              C_WARNING);
          game->connection.server_close();
        } else {
          game->console.write("Already in client mode...", C_ERROR);
          break;
        }
      }
      if (game->connection.port_open(DEFAULT_CLIENT_PORT)) {
        if (!game->connection.fecth_public_ip())
          game->console.write("No internet connection, local network only",
                              C_ERROR);

      } else
        game->console.write("Failed to open port 1337...", C_ERROR);

      game->console.write("Client mode initiated", C_SUCCESS);

      game->host = false;

      break;
    }

    case con_init_client_localhost: {
      if (game->connection.port_is_open()) {
        if (game->connection.server_is_open()) {
          game->console.write("Server mode detected, closing server...",
                              C_WARNING);
          game->connection.server_close();
        } else {
          game->console.write("Already in client mode...", C_ERROR);
          break;
        }
      }
      if (!game->connection.port_open(DEFAULT_CLIENT_PORT))
        game->console.write("Failed to open port 1337...", C_ERROR);

      game->console.write("Client mode initiated", C_SUCCESS);

      game->host = false;

      break;
    }

    case con_host_game: {
      if (game->connection.server_is_open()) {
        ship_mp_spawn(&game->player1, &game->player2, true);
        game->state = MP_WARMUP;

      } else
        game->console.write(
            "You need to start a server to host a game, use init_server 'port'",
            C_ERROR);

      game->host = true;

      game->console.close();

      break;
    }

    case con_connect: {
      if (game->connection.server_is_open() ||
          !game->connection.port_is_open()) {
        game->console.write(
            "To connect to a game use client mode by entering 'init_client' in "
            "the console",
            C_ERROR);
        break;
      }

      std::string ip;
      std::string portstr;
      getline(str, ip, ':');
      getline(str, portstr);
      uint16_t port;
      try {
        port = std::stoi(portstr);
      } catch (const std::invalid_argument&) {
        game->console.write("Invalid address format", C_ERROR);
        return;
      }

      if (ip.empty() || portstr.empty()) {
        game->console.write("Invalid address format", C_ERROR);
        return;
      }

      if (!game->connection.resolve_ip(ip, port)) {
        game->console.write("Failed to resolve address: " + str.str(), C_ERROR);
        return;
      }

      net_message info;
      info.playern = game->player1;
      info.header = "client";
      info.ip = game->connection.get_public_ip();
      info.port = game->connection.get_port();
      info.timer_tick = game->timers.timerTick;

      game->connection.send_packet(info);

      ship_mp_spawn(&game->player1, &game->player2, false);
      game->host = false;
      game->state = MP_WARMUP;
      game->threads.push(start_transmitting, std::ref(*game));
      game->threads.push(start_listening, std::ref(*game));

      break;
    }

    case con_disconnect: {
      if (game->connection.server_is_open() ||
          !game->connection.port_is_open()) {
        game->connection.server_close();
        game->connection.close();
        game->connection.port_close();
        game->state = MENU;
        break;
      } else
        game->console.write("Not currently in a online game", C_ERROR);

      break;
    }

    case con_send_testpacket: {
      net_message message;
      message.header = "info packet";
      message.timer_tick = game->timers.timerTick;

      if (game->connection.is_connected()) {
        game->connection.send_packet(message);
        game->console.write("Packet sent", C_SUCCESS);
      } else {
        game->console.write(
            "Initiate network and connect to a remote peer first", C_ERROR);
        game->console.write("Use init_server portnumber or init_client",
                            C_NORMAL);
        game->console.write("Use connect IP:PORT to establish a connection",
                            C_NORMAL);
      }

      break;
    }

    case con_receive_testpacket: {
      game->threads.push(start_listening, std::ref(*game));
      break;
    }

    case con_set_name: {
      std::string name;
      getline(str, name);
      if (name.size() > 14) {
        game->console.write("Name is too big, 14 characters maximum", C_ERROR);
        break;
      }

      game->player1.name = name;
      game->console.write("Name set to: " + game->player1.name, C_NORMAL);

      break;
    }
    case con_help: {
      game->console.write("quit                   - Exit game", C_WHITE);
      game->console.write("clear                  - Clear console", C_WHITE);
      game->console.write(
          "init_server portnumber - Initiates server listening to given port "
          "number",
          C_WHITE);
      game->console.write(
          "init_client            - Initiates network in client mode", C_WHITE);
      game->console.write(
          "host_game              - Initiates network game if server mode has "
          "been started",
          C_WHITE);
      game->console.write(
          "connect ip:port        - Connects to a network game hosted by a "
          "server with given ip and port number",
          C_WHITE);
      game->console.write(
          "disconnect             - Disconnects from a network game", C_WHITE);
      game->console.write("set_name name          - Sets player name", C_WHITE);
      game->console.write("help                   - Duh", C_WHITE);
      break;
    }

    /* General commands */
    case con_quit: {
      if (game->connection.server_is_open()) game->connection.server_close();
      game->connection.close();
      game->state = COMP;
      break;
    }
    case con_clear: {
      game->console.clear();
      break;
    }
    default:
      return;
  }
}
