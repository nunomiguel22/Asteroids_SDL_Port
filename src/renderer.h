#pragma once
#include <stdint.h>


#include "game.h"
#include "mvector.h"

uint8_t* init_sdl(game_settings *settings);
void  exit_sdl();
void reset_sdl(game_settings *settings);
int draw_pixel(int x, int y, uint32_t color);
void display_frame();

void show_splash(game_data *game);

void draw_char(unsigned int x, unsigned int y, char character, uint32_t color, double scale, bitmap_data *bmp);
void draw_string(unsigned int x, unsigned int y, std::string line, uint32_t color, double scale, bitmap_data *bmp);

void draw_digit(unsigned int num, int x, int y, game_data *game);
void draw_large_digit(unsigned int num, int x, int y, game_data *game);
void draw_number(int number, int x, int y, game_data *game);
void draw_ship(Bitmap *bmp, player *p);
void draw_alien(game_data *game);
void draw_ast(asteroid *ast, Bitmap *bmp);

void render_frame(game_data *game);
void render_seq_frame(game_data *game);
void render_console(game_data * game);

void handle_frame(game_data *game);
void handle_menu_frame(game_data *game, Bitmap *bckgrd);

mpoint2d vector_translate_gfx(mpoint2d *cartesian, unsigned int width, unsigned int height);
