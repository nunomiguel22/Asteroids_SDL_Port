#pragma once
#include <stdint.h>


#include "game.h"
#include "mvector.h"
#include "console.h"

/* SDL functions */
uint32_t* init_sdl(game_settings *settings, console *cons);
void  exit_sdl(game_data *game);
void reset_sdl(game_settings *settings);

/* Low level functions */
mpoint2d vector_translate_gfx(mpoint2d *cartesian, unsigned int width, unsigned int height);
int draw_pixel(int x, int y, uint32_t color);				//Draw to pixel buffer
void display_frame();										//Show frame on screen

/* Render entities */
void render_hp_bar(player *p, int x, int y);
void render_laser(player *p, Bitmap *laser, weapon *l);
void draw_ship(Bitmap *bmp, player *p);
void draw_alien(game_data *game);
void draw_ast(asteroid *ast, Bitmap *bmp);
void render_crosshair(Bitmap *delta, player *player1);

/* Render scenes*/
void show_splash(game_data *game);
void render_frame(game_data *game);
void render_seq_frame(game_data *game);
void render_console(game_data * game);

void handle_frame(game_data *game);
void handle_frame_wrapper(int id, game_data &game);

void handle_menu_frame(game_data *game, Bitmap *bckgrd);


