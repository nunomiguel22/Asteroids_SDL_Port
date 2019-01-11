#pragma once
#include <stdint.h>
#include "game.h"
#include "mvector.h"

uint8_t * init_sdl();

int draw_pixel(int x, int y, uint32_t color);
void display_frame();

void show_splash(game_data *game);

void draw_digit(unsigned int num, int x, int y, game_data *game);
void draw_large_digit(unsigned int num, int x, int y, game_data *game);
void draw_number(int number, int x, int y, game_data *game);
void draw_ship(Pixmap *xpm, player *p);
void draw_alien(game_data *game);
void draw_ast(asteroid *ast, Pixmap *xpm);

void render_frame(game_data *game);
void render_seq_frame(game_data *game);







mpoint2d vector_translate_gfx(mpoint2d *vector_space, unsigned int screen_width, unsigned int screen_height);
