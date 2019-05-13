#pragma once

/* GAME MACROS */

//Technical
#define BIT(n) (0x01<<(n))
#define GAME_VERSION "2.0"
#define AUTHOR "Nuno Marques"
#define FILE_UI_PATH "Assets/ui/"
#define FILE_TEXTURES_PATH "Assets/textures/"
#define FILE_ANIMATIONS_PATH "Assets/animations/"
#define FILE_SOUNDS_PATH "Assets/sounds/"
#define FILE_FONTS_PATH "Assets/fonts/"
#define DEFAULT_CLIENT_PORT 1337
#define UDP_PACKET_SIZE 512

//Ship
#define FIRE_RATE 20 //Ticks inbeween rounds
#define JUMP_RATE 3 //Seconds before another random jump is available
#define MAIN_THRUSTER_ACCELARATION 2
#define MAIN_REVERSE_ACCELARATION 0.5
#define PORT_THRUSTER_ACCELARATION 0.5
#define STARBOARD_THRUSTER_ACCELARATION 0.5
#define THRUSTERS_MAXIMUM_VELOCITY 4 //Overall ship's maximum velocity
#define SHIP_HITRADIUS 30
#define AMMO 5 //Maximum bullets on screen at any moment
#define LASER_VELOCITY 12 //Projectile travel speed
#define PLAYER_MAX_HEALTH 100 //Maximum player health points
#define PLAYER_HEALTH_REGENERATION 10 //Player health points gain per round
#define PLAYER_LASER_DAMAGE 30

//Alien ship
#define ALIEN_MAX_HEALTH 150
#define ALIEN_FIRE_RATE 1.5
#define ALIEN_HIT_RADIUS 60
#define ALIEN_MAX_ACCELARATION 2
#define ALIEN_MIN_ACCELARATION 1
#define ALIEN_MAX_VELOCITY 3
#define ALIEN_DEATH_DURATION 0.5
#define ALIEN_SPAWN_CHANCE_INCREASE 20
#define ALIEN_ROTATION_SPEED 2

#define DELAY_BETWEEN_ROUNDS 40

//Asteroids
#define MAX_ASTEROIDS 20 //Maximum number of asteroids on screen at any time
#define STARTING_ASTEROIDS 3 //Number of asteroids on the first round
#define ASTEROID_INCREASE_RATE 1 //Increase in asteroid count per round
#define SMALL_ASTEROID_CHANGE_INCREASE_RATE 3 //Increase in chance of small asteroid every round
#define ASTEROID_DEATH_DURATION 20 //Durantion of asteroid destruction animation in seconds
#define MEDIUM_ASTEROID_HITRADIUS 20
#define MEDIUM_ASTEROID_MAX_VELOCITY 4
#define MEDIUM_ASTEROID_MIN_VELOCITY 3
#define LARGE_ASTEROID_HITRADIUS 35
#define LARGE_ASTEROID_MAX_VELOCITY 2
#define LARGE_ASTEROID_MIN_VELOCITY 1

//1024x768 bounds
#define hres 1024
#define vres 768
#define hres_center 512
#define vres_center 384

#define math_h_negative_bound -512
#define math_h_positive_bound 511
#define math_v_negative_bound -383
#define math_v_positive_bound 384

//Console
#define CONSOLE_VERSION "0.95"
#define CONSOLE_DISPLAY_LIMIT 28
#define CONSOLE_INPUT_LIMT 30
#define VERT_BAR_Y_POS 439
#define CON_FONT_SIZE 12
#define LARGE_FONT_SIZE 14
#define MASSIVE_FONT_SIZE 40
#define C_GREEN 0x00FF00FF
#define C_RED 0xFF0000FF
#define C_BLUE 0x0000FFFF
#define C_YELLOW 0xFFFF00FF
#define C_WHITE 0xFFFFFFFF
#define C_BLACK 0x000000FF
#define C_LIGHTGRAY 0xCCCCCCFF
#define C_ERROR 0xFF4444FF
#define C_NORMAL 0x99CCFFFF
#define C_WARNING 0xFFFF00FF
#define C_SUCCESS 0x44FF44FF
