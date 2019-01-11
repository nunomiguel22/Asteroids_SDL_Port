#include "vcard.h"
#include <iostream>
#include "SDL.h"
#include "macros.h"
#include "windows.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer;
SDL_Texture *screen;
uint8_t *pixelbuffer;
const int pixel_bytes = 4;
const int vram_size = hres * vres * pixel_bytes;

using namespace std;

uint8_t * init_sdl() {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {

		cout << "SDL could not initialize! SDL_Error:" << SDL_GetError() << endl;

		return NULL;
	}

	SDL_CreateWindowAndRenderer(hres, vres, SDL_WINDOW_SHOWN, &window, &renderer);
	screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, hres, vres);
	pixelbuffer = (uint8_t *)malloc(vram_size);
	SDL_RaiseWindow(window);
	SDL_ShowCursor(SDL_DISABLE);

	return pixelbuffer;
}

int draw_pixel(int x, int y, uint32_t color) {

	uint8_t *v_address = pixelbuffer;

	if (x > hres - 1 || y > vres - 1 || x < 0 || y < 0) {
		return 1;
	}

	v_address += ((hres * y) + x) * pixel_bytes;

	for (unsigned int i = 0; i < pixel_bytes; i++) {
		*v_address = color >> (8 * i);
		v_address++;
	}

	return 0;
}

void display_frame() {

	SDL_UpdateTexture(screen, NULL, pixelbuffer, hres * 4);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, screen, NULL, NULL);
	SDL_RenderPresent(renderer);
	memset(pixelbuffer, 0, vram_size);
}


void show_splash(game_data *game) {
	game->bmp.splash.draw(0, 0);
	display_frame();
	Sleep(2000);
	display_frame();
	game->bmp.menu.draw(0, 0);
	game->xpm.cursor.draw(hres / 2, vres / 2, false);
	display_frame();
}

void draw_digit(unsigned int num, int x, int y, game_data *game)
{
	switch (num) {
	case 0: game->xpm.n_zero.draw(x, y, false); break;
	case 1: game->xpm.n_one.draw(x, y, false); break;
	case 2: game->xpm.n_two.draw(x, y, false); break;
	case 3: game->xpm.n_three.draw(x, y, false); break;
	case 4: game->xpm.n_four.draw(x, y, false); break;
	case 5: game->xpm.n_five.draw(x, y, false); break;
	case 6: game->xpm.n_six.draw(x, y, false); break;
	case 7: game->xpm.n_seven.draw(x, y, false); break;
	case 8: game->xpm.n_eight.draw(x, y, false); break;
	case 9: game->xpm.n_nine.draw(x, y, false); break;

	default: break;
	}
}

void draw_large_digit(unsigned int num, int x, int y, game_data *game) {

	switch (num) {
	case 1:	game->xpm.n_one_large.draw(x, y, false); break;
	case 2: game->xpm.n_two_large.draw(x, y, false); break;
	case 3: game->xpm.n_three_large.draw(x, y, false); break;
	default: break;
	}
}

void draw_number(int number, int x, int y, game_data *game) {

	int temp_number = number;
	int size = 0;
	int reverse_number = 0;

	while (true) {
		size++;
		reverse_number = reverse_number * 10 + (temp_number % 10);
		temp_number = floor(temp_number / 10);
		if (temp_number == 0)
			break;
	}

	for (int i = 0; i < size; i++) {
		draw_digit(reverse_number % 10, x, y, game);
		reverse_number = floor(reverse_number / 10);
		x += 10;
	}
}

void render_seq_frame(game_data *game) {

	game->bmp.game_background.draw(0, 0);
	draw_ship(&game->xpm.ship_blue, &game->player1);
	draw_large_digit(game->timers.start_seq, 512, 304, game);
}

void draw_ship(Pixmap *xpm, player *p) {
	double degrees;

	mvector2d vmouse(p->pivot, p->crosshair);
	mvector2d vcannon(p->pivot, p->cannon);
	
	if (vmouse.magnitude() > 5)
		degrees =vmouse.angle() - 90;
	else degrees = vmouse.angle() - 90;

	mpoint2d ws_pivot = vector_translate_gfx(&p->pivot, 1024, 768);

	xpm->draw(ws_pivot.x, ws_pivot.y, degrees);
}


mpoint2d vector_translate_gfx(mpoint2d *vector_space, unsigned int screen_width, unsigned int screen_height) {

	mpoint2d gfx_point;
	mpoint2d origin;
	origin.x = screen_width / 2;
	origin.y = screen_height / 2;

	gfx_point.x = origin.x + vector_space->x;
	gfx_point.y = origin.y - vector_space->y;

	return gfx_point;
}