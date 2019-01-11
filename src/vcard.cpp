#include "vcard.h"
#include <iostream>
#include "SDL.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer;
SDL_Texture *screen;
uint8_t *pixelbuffer;
const int hres = 1024;
const int vres = 768;
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
}


