#include <fstream>
#include <sstream>

#include "graphics.h"
#include "renderer.h"
#include "mvector.h"

#define BMP_ID 0x4D42

Bitmap::~Bitmap() {}
DIB_Header Bitmap::get_dib_header() const { return dibheader; }
uint32_t* Bitmap::get_data() { return pixel_data; }


int Bitmap::load(std::string filepath) {

	/*
		Notes: Since we're doing 4 bytes per pixel, the number of bytes in a row is always a multiple of 4, so
		there is no need to check for padding
	*/

	std::ifstream bmp;

	bmp.open(filepath, std::ifstream::binary);

	if (!bmp.is_open())
		return 1;

	bmp.seekg(0, bmp.end);
	int length = (int)bmp.tellg();
	bmp.seekg(0, bmp.beg);

	char * buffer = new char[length];
	bmp.read(buffer, length);
	bmp.close();

	int index = 0;

	/* BMP HEADER*/

	memcpy(&bmpheader.ID_field, buffer + index, 4);

	// Check if it's a bmp file
	if (bmpheader.ID_field != BMP_ID)
		return 2;

	index += 2;
	memcpy(&bmpheader.size, buffer + index, 4);
	index += 4;
	memcpy(&bmpheader.application_specific, buffer + index, 4);
	index += 4;
	memcpy(&bmpheader.offset, buffer + index, 4);
	
	/* DIB HEADER */

	index += 4;
	memcpy(&dibheader.dib_size, buffer + index, 4);
	index += 4;
	memcpy(&dibheader.width, buffer + index, 4);
	index += 4;
	memcpy(&dibheader.height, buffer + index, 4);
	index += 4;
	memcpy(&dibheader.planes, buffer + index, 2);
	index += 2;
	memcpy(&dibheader.bits_per_pixel, buffer + index, 2);
	index += 2;
	memcpy(&dibheader.compression, buffer + index, 4);
	index += 4;
	memcpy(&dibheader.raw_data_size, buffer + index, 4);
	index += 4;
	memcpy(&dibheader.h_pixels_per_meter, buffer + index, 4);
	index += 4;
	memcpy(&dibheader.v_pixels_per_meter, buffer + index, 4);
	index += 4;
	memcpy(&dibheader.colors_in_pallete, buffer + index, 4);
	index += 4;
	memcpy(&dibheader.important_colors, buffer + index, 4);
	index += 4;
	memcpy(&dibheader.red_bitmask, buffer + index, 4);
	index += 4;
	memcpy(&dibheader.green_bitmask, buffer + index, 4);
	index += 4;
	memcpy(&dibheader.blue_bitmask, buffer + index, 4);
	index += 4;
	memcpy(&dibheader.alpha_bitmask, buffer + index, 4);

	bytes_per_pixel = dibheader.bits_per_pixel >> 3;
	int bytes_per_line = dibheader.width * bytes_per_pixel;

	/* PIXEL DATA */
	
	/* Copy data to temporary buffer */
	uint32_t *temp_data = (uint32_t *)malloc(dibheader.raw_data_size);
	pixel_data = (uint32_t *)malloc(dibheader.raw_data_size);
	memcpy(temp_data, buffer + bmpheader.offset, dibheader.raw_data_size);

	/* Copy to final buffer by inverting data to start at the top left pixel */
	for (unsigned int i = 0; i < dibheader.height; ++i)
		memcpy(pixel_data + ((dibheader.height - (i + 1)) * dibheader.width), temp_data + (i*dibheader.width), bytes_per_line);

	free(temp_data);
	free(buffer);
	return 0;
}

void Bitmap::draw(int x, int y) {

	if (pixel_data == NULL)
		return;

	if (x < 0 || x + dibheader.width > hres || y < 0 || y + dibheader.height > vres)
		return;

	for (unsigned int i = 0; i < dibheader.height; ++i)
		for (unsigned int j = 0; j < dibheader.width; ++j) {
			uint32_t color = pixel_data[(i * dibheader.width) + j];
			draw_pixel(x + j, y + i, color);
		}
}

void  Bitmap::draw_transform(int gx, int gy, double rotation_degrees, BMP_ALIGN alignment) {

	if (pixel_data == NULL)
		return;

	int height = dibheader.height;
	int width = dibheader.width;
	int y_comp = 0;
	int x_comp = 0;
	if (alignment = BMP_CENTER) {
		x_comp = width / 2;
		y_comp = height / 2;
	}

	if (gx < 0 || gx > hres || gy  < 0 || gy> vres)
		return;

	int center_x = (width / 2);
	int center_y = (height / 2) + 1;

	int limit = 0;
	if (dibheader.height > dibheader.width)
		limit = (int)(dibheader.height * 1.5);
	else limit = (int)(dibheader.width * 1.5);


	for (unsigned int i = 0; i < dibheader.height - 1; ++i)
		for (unsigned int j = 0; j < dibheader.width - 1; ++j) {
			uint32_t color = 0;
			int x = j - center_x;
			int y = center_y - i;
			mvector2d point(x, y);
			point.rotate(-rotation_degrees);
			x = (int)(round(point.getX() + center_x));
			y = (int)(round(center_y - point.getY()));

			if (x < (int)dibheader.width - 1 && x > 0 && y > 0 && y < (int)dibheader.height - 1)
				color = pixel_data[(y * dibheader.width) + x];
			draw_pixel(gx + j - x_comp, gy + i - y_comp, color);
		}
}

int load_bitmaps(bitmap_data *bmp, console *cons) {

	std::string file;
	cons->write_to_log("LOADING BMP ASSETS");

	/* User interface elements*/

	file = FILE_UI_PATH"splash.bmp";
	if (bmp->splash.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_UI_PATH"MenuBackground.bmp";
	if (bmp->menubackground.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_UI_PATH"ui_status.bmp";
	if (bmp->ui_status.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_UI_PATH"highscoresbackground.bmp";
	if (bmp->hsbackground.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_UI_PATH"playbutton.bmp";
	if (bmp->playbutton.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_UI_PATH"optionsbutton.bmp";
	if (bmp->optionsbutton.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_UI_PATH"quitbutton.bmp";
	if (bmp->quitbutton.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_UI_PATH"options.bmp";
	if (bmp->options.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_UI_PATH"deathscreen.bmp";
	if (bmp->death_screen.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_UI_PATH"deathscreenhighscore.bmp";
	if (bmp->death_screen_highscore.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_UI_PATH"boxticked.bmp";
	if (bmp->boxticked.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_UI_PATH"pausemessage.bmp";
	if (bmp->pause_message.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_UI_PATH"parrow.bmp";
	if (bmp->p_arrow.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_UI_PATH"cursor.bmp";
	if (bmp->cursor.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_UI_PATH"console.bmp";
	if (bmp->gameconsole.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_UI_PATH"slidemarker.bmp";
	if (bmp->slidemarker.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");


	/* Textures */

	file = FILE_TEXTURES_PATH"gamebackground.bmp";
	if (bmp->game_background.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_TEXTURES_PATH"alienship.bmp";
	if (bmp->alien_ship.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_TEXTURES_PATH"mediumscore.bmp";
	if (bmp->medium_score.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_TEXTURES_PATH"largescore.bmp";
	if (bmp->large_score.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_TEXTURES_PATH"alienscore.bmp";
	if (bmp->alien_score.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_TEXTURES_PATH"LargeAsteroid.bmp";
	if (bmp->large_asteroid.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_TEXTURES_PATH"MediumAsteroid.bmp";
	if (bmp->medium_asteroid.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_TEXTURES_PATH"pix_ship_blue.bmp";
	if (bmp->pix_ship_blue.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_TEXTURES_PATH"pix_ship_blue_bt.bmp";
	if (bmp->pix_ship_blue_bt.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_TEXTURES_PATH"pix_ship_blue_pt.bmp";
	if (bmp->pix_ship_blue_pt.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_TEXTURES_PATH"pix_ship_blue_st.bmp";
	if (bmp->pix_ship_blue_st.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_TEXTURES_PATH"pix_ship_blue_teleport.bmp";
	if (bmp->pix_ship_blue_tele.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_TEXTURES_PATH"delta.bmp";
	if (bmp->delta.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_TEXTURES_PATH"delta_hit.bmp";
	if (bmp->delta_hit.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_TEXTURES_PATH"laser_blue.bmp";
	if (bmp->laser_blue.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_TEXTURES_PATH"laser_red.bmp";
	if (bmp->laser_red.load(file)) {
		cons->write_to_log("Couldn't load \"" + file + "\"");
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");


	/* Animations */

	for (int i = 1; i < 11; ++i) {
		std::stringstream ss1;
		ss1 << FILE_ANIMATIONS_PATH "/explosion/exp" << i << ".bmp";
		if (bmp->ship_expl[i - 1].load(ss1.str())) {
			cons->write_to_log("Couldn't load \"" + ss1.str() + "\"");
			return 1;
		}
		else cons->write_to_log("Loaded \"" + ss1.str() + "\"");

	}

	for (int i = 1; i < 11; ++i) {
		std::stringstream ss2;
		ss2 << "Assets/animations/astdest/astdest" << i << ".bmp";
		if (bmp->ast_dest[i - 1].load(ss2.str())) {
			cons->write_to_log("Couldn't load \"" + ss2.str() + "\"");
			return 1;
		}
		else cons->write_to_log("Loaded \"" + ss2.str() + "\"");
	}

	cons->write_to_log("BMP ASSETS LOADED");
	cons->write_to_log(" ");

	return 0;
}


void free_bitmaps(bitmap_data *bmp) {
	

	free(bmp->menubackground.get_data());
	free(bmp->ui_status.get_data());
	free(bmp->hsbackground.get_data());
	free(bmp->playbutton.get_data());
	free(bmp->optionsbutton.get_data());
	free(bmp->quitbutton.get_data());
	free(bmp->options.get_data());
	free(bmp->death_screen.get_data());
	free(bmp->death_screen_highscore.get_data());
	free(bmp->boxticked.get_data());
	free(bmp->game_background.get_data());
	free(bmp->pause_message.get_data());
	free(bmp->splash.get_data());
	free(bmp->alien_ship.get_data());
	free(bmp->medium_score.get_data());
	free(bmp->large_score.get_data());
	free(bmp->alien_score.get_data());
	free(bmp->large_asteroid.get_data());
	free(bmp->medium_asteroid.get_data());
	free(bmp->pix_ship_blue.get_data());
	free(bmp->pix_ship_blue_bt.get_data());
	free(bmp->pix_ship_blue_pt.get_data());
	free(bmp->pix_ship_blue_st.get_data());
	free(bmp->delta.get_data());
	free(bmp->delta_hit.get_data());
	free(bmp->laser_blue.get_data());
	free(bmp->laser_red.get_data());
	free(bmp->p_arrow.get_data());
	free(bmp->cursor.get_data());
	free(bmp->gameconsole.get_data());
	free(bmp->slidemarker.get_data());

	for (int i = 0; i < 10; ++i) {
		free(bmp->ship_expl[i].get_data());
	}
	for (int i = 0; i < 3; ++i) {
		free(bmp->ast_dest[i].get_data());
	}
}

/* FONTS */


Font::Font(std::string filepath, int sz) {
	font = TTF_OpenFont(filepath.c_str(), sz);
	size = sz;
}

Font::~Font() { TTF_CloseFont(font); }

TTF_Font * Font::get_font_data() const { return font; }
int Font::get_size() const { return size; }

int Font::load(std::string filepath, int sz) {
	font = TTF_OpenFont(filepath.c_str(), sz);
	if (font == NULL)
		return 1;
	size = sz;
	return 0;
}

uint32_t Font::get_surface_pixel(SDL_Surface *surface, int x, int y) {

	/* Got this function from http://sdl.beuc.net/sdl.wiki/Pixel_Access */

	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16 *)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32 *)p;
		break;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
}

void Font::render_string(unsigned int x, unsigned int y, std::string line, uint32_t color) {
	
	if (line.empty())
		return;

	SDL_Color unimportant_color;
	unimportant_color.r = 99;
	unimportant_color.g = 0xCC;
	unimportant_color.b = 0xFF;

	SDL_Surface* tempsurf = TTF_RenderText_Solid(font, line.c_str(), unimportant_color);

	for (int i = 0; i < tempsurf->h; ++i)
		for (int j = 0; j < tempsurf->w; ++j)
			if (get_surface_pixel(tempsurf, j, i))
				draw_pixel(x + j, y + i, color);

	SDL_FreeSurface(tempsurf);
}

void Font::render_string(unsigned int x, unsigned int y, std::string line, uint32_t fg_color, uint32_t bg_color) {

	if (line.empty())
		return;

	SDL_Color unimportant_color;
	unimportant_color.r = 99;
	unimportant_color.g = 0xCC;
	unimportant_color.b = 0xFF;

	SDL_Surface* tempsurf = TTF_RenderText_Solid(font, line.c_str(), unimportant_color);

	for (int i = 0; i < tempsurf->h; ++i)
		for (int j = 0; j < tempsurf->w; ++j)
			if (get_surface_pixel(tempsurf, j, i))
				draw_pixel(x + j, y + i, fg_color);
			else draw_pixel(x + j, y + i, bg_color);

	SDL_FreeSurface(tempsurf);

}

void Font::render_number(double number, int x, int y, uint32_t color) {

	std::stringstream ss;
	ss << number;

	SDL_Color unimportant_color;
	unimportant_color.r = 99;
	unimportant_color.g = 0xCC;
	unimportant_color.b = 0xFF;

	SDL_Surface* tempsurf = TTF_RenderText_Solid(font, ss.str().c_str(), unimportant_color);

	for (int i = 0; i < tempsurf->h; ++i)
		for (int j = 0; j < tempsurf->w; ++j)
			if (get_surface_pixel(tempsurf, j, i))
				draw_pixel(x + j, y + i, color);

	SDL_FreeSurface(tempsurf);
}

