#pragma once
#include <stdint.h>

uint8_t * init_sdl();

int draw_pixel(int x, int y, uint32_t color);

void display_frame();

