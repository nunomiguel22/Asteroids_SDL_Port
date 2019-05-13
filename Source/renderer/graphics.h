#pragma once
#include <stdint.h>
#include <string>
#include <SDL2/SDL_ttf.h>

#include "../general/console.h"
#include "../general/macros.h"

typedef enum { BMP_TOPLEFT, BMP_CENTER } BMP_ALIGN;

typedef struct {
	uint16_t ID_field;
	uint32_t size;
	uint32_t application_specific;
	unsigned int offset;
} BMP_Header;

typedef struct {

	uint32_t dib_size; 
	uint32_t width;
	uint32_t height;
	uint16_t planes;
	uint16_t bits_per_pixel;
	uint32_t compression;
	uint32_t raw_data_size;
	uint32_t h_pixels_per_meter;
	uint32_t v_pixels_per_meter;
	uint32_t colors_in_pallete;
	uint32_t important_colors;
	uint32_t red_bitmask;
	uint32_t green_bitmask;
	uint32_t blue_bitmask;
	uint32_t alpha_bitmask;
} DIB_Header;

class Bitmap {
	BMP_Header bmpheader;
	DIB_Header dibheader;
	uint32_t *pixel_data;
	uint8_t bytes_per_pixel;
public:
	Bitmap() {};
	~Bitmap();
	DIB_Header get_dib_header() const;
	uint32_t* get_data();

	int load(std::string filepath);
	void draw(int x, int y);
	void draw_transform(int gx, int gy, float rotation_degrees, BMP_ALIGN alignment);
	void draw_background(int x, int y);
};

/* TTF_Font wrapper class */

class Font {
	TTF_Font *font;
	int f_size;
public:
	Font() {};
	Font(std::string filepath, int sz);
	~Font();

	TTF_Font * get_font_data() const;
	int size() const;

	int load(std::string filepath, int sz);
	uint32_t get_surface_pixel(SDL_Surface *surface, int x, int y);
	void render_string(unsigned int x, unsigned int y, std::string line, uint32_t color);
	void render_string(unsigned int x, unsigned int y, std::string line, uint32_t fg_color, uint32_t bg_color);
	void render_number(float number, int x, int y, uint32_t color);
};

/* GAME DATA */

typedef struct {

	Bitmap menubackground;
	Bitmap mp_background;
	Bitmap mp_hpbaroutline;
	Bitmap mp_nameplate;
	Bitmap mp_scoreplate;
	Bitmap ui_status;
	Bitmap cursor;
	Bitmap hsbackground;
	Bitmap gameconsole;
	Bitmap options;
	Bitmap boxticked;
	Bitmap game_background;
	Bitmap pause_message;
	Bitmap splash;
	Bitmap death_screen;
	Bitmap death_screen_highscore;
	Bitmap medium_score;
	Bitmap large_score;
	Bitmap alien_score;
	Bitmap alien_ship;
	Bitmap large_asteroid;
	Bitmap medium_asteroid;
	Bitmap pix_ship_blue;
	Bitmap pix_ship_blue_bt;
	Bitmap pix_ship_blue_st;
	Bitmap pix_ship_blue_pt;
	Bitmap pix_ship_blue_tele;
	Bitmap pix_ship_alt;
	Bitmap pix_ship_alt_bt;
	Bitmap pix_ship_alt_st;
	Bitmap pix_ship_alt_pt;
	Bitmap pix_ship_alt_tele;
	Bitmap delta;
	Bitmap delta_hit;
	Bitmap p_arrow;
	Bitmap slidemarker;
	Bitmap ship_expl[10];					//Animation
	Bitmap ast_dest[10];					//Animation
	Bitmap laser_blue;
	Bitmap laser_red;
	Bitmap nm_button;
	Bitmap hl_button;

}bitmap_data;

typedef struct {

	Font lucida_console_med;
	Font copperplategothicbold;
	Font copperplategothicbold_massive;
}font_data;

/* Auxiliary */

int load_bitmaps(bitmap_data *bmp, console *cons);
void free_bitmaps(bitmap_data *bmp);