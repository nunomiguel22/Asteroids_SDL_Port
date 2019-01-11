#include <iostream>
#include <SDL.h>
#include <Windows.h>
#include "vcard.h"
#include "graphics.h"


using namespace std;

int main(int argc, char *argv[]) {

	uint8_t *pixbuffer = init_sdl();

	bitmap_data test;
	load_bitmaps(&test);
	test.splash.draw(0,0, pixbuffer);

	display_frame();

	Sleep(5000);


	return 0;

}