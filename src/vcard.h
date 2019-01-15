#pragma once
#include <stdint.h>
#include "game.h"
#include "mvector.h"

typedef struct {
	uint32_t hresolution;
	uint32_t vresolution;
	bool fullscreen;
	bool fullscreennative;
	bool borderless;
	bool vsync;
}video_settings;


void read_video_settings(video_settings *settings);

uint8_t* init_sdl(video_settings *settings);
void  exit_sdl();

int draw_pixel(int x, int y, uint32_t color);
void display_frame();

void show_splash(game_data *game);

void draw_digit(unsigned int num, int x, int y, game_data *game);
void draw_large_digit(unsigned int num, int x, int y, game_data *game);
void draw_number(int number, int x, int y, game_data *game);
void draw_ship(Bitmap *bmp, player *p);
void draw_alien(game_data *game);
void draw_ast(asteroid *ast, Bitmap *bmp);

void render_frame(game_data *game);
void render_seq_frame(game_data *game);



mpoint2d vector_translate_gfx(mpoint2d *vector_space, unsigned int screen_width, unsigned int screen_height);
