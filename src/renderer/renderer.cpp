#include <SDL.h>

#include "renderer.h"
#include "macros.h"
#include <string>
#include <queue>
#include "SDL_ttf.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer;
SDL_Texture *screen;
uint8_t *pixelbuffer;

const int pixel_bytes = 4;
const int vram_size = hres * vres * pixel_bytes;


uint8_t * init_sdl(game_settings *settings) {

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return NULL;

	uint32_t windowFlags = SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS;

	if (settings->fullscreen)
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	else if (settings->fullscreennative)
		windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	else if (settings->borderless)
		windowFlags |= SDL_WINDOW_BORDERLESS;

	uint32_t rendererFlags = SDL_RENDERER_ACCELERATED;

	if (settings->vsync)
		rendererFlags |= SDL_RENDERER_PRESENTVSYNC;

	window = SDL_CreateWindow("Asteroids", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, settings->hresolution, settings->vresolution, windowFlags);
	renderer = SDL_CreateRenderer(window, -1, rendererFlags);

	screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, hres, vres);
	pixelbuffer = (uint8_t *)malloc(vram_size);
	SDL_RaiseWindow(window);
	SDL_ShowCursor(SDL_DISABLE);

	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
		return NULL;

	if (TTF_Init() < 0)
		return NULL;



	return pixelbuffer;
}

void  exit_sdl(game_data *game) {

	TTF_CloseFont(game->lucida_console_med);
	TTF_Quit();
	free(pixelbuffer);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(screen);
	SDL_Quit();
}

void reset_sdl(game_settings *settings) {

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(screen);

	uint32_t windowFlags = SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS;

	if (settings->fullscreen)
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	else if (settings->fullscreennative)
		windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	else if (settings->borderless)
		windowFlags |= SDL_WINDOW_BORDERLESS;

	uint32_t rendererFlags = SDL_RENDERER_ACCELERATED;

	if (settings->vsync)
		rendererFlags |= SDL_RENDERER_PRESENTVSYNC;

	window = SDL_CreateWindow("Asteroids", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, settings->hresolution, settings->vresolution, windowFlags);
	renderer = SDL_CreateRenderer(window, -1, rendererFlags);

	screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, hres, vres);
}

int draw_pixel(int x, int y, uint32_t color) {

	uint8_t *v_address = pixelbuffer;

	if (x > hres - 1 || y > vres - 1 || x < 0 || y < 0) 
		return 1;

	v_address += ((hres * y) + x) * pixel_bytes;

	for (unsigned int i = 0; i < pixel_bytes; i++) {
		*v_address = color >> (8 * i);
		v_address++;
	}

	return 0;
}

void display_frame() {

	SDL_UpdateTexture(screen, NULL, pixelbuffer, hres * 4);
	SDL_RenderCopy(renderer, screen, NULL, NULL);
	SDL_RenderPresent(renderer);
	
	memset(pixelbuffer, 0, vram_size);
}


void show_splash(game_data *game) {
	game->bmp.splash.draw(0, 0);
	display_frame();
	SDL_Delay(3000);
	display_frame();
	game->bmp.menubackground.draw(0, 0);
	game->xpm.cursor.draw(hres / 2, vres / 2);
	display_frame();
}

uint32_t get_surface_pixel(SDL_Surface *surface, int x, int y) {
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


void draw_string(unsigned int x, unsigned int y, std::string line, uint32_t color, TTF_Font *font) {
	
	if (line.empty())
		return;

	SDL_Color col;
	col.r = 99;
	col.g = 0xCC;
	col.b = 0xFF;

	SDL_Surface* test = TTF_RenderText_Solid(font, line.c_str(), col);

	for (int i = 0; i < test->h; ++i) 
		for (int j = 0; j < test->w; ++j) 
			if (get_surface_pixel(test, j, i)) 
				draw_pixel(x + j, y + i, color);


}


void draw_digit(unsigned int num, int x, int y, game_data *game)
{
	switch (num) {
	case 0: game->xpm.n_zero.draw(x, y); break;
	case 1: game->xpm.n_one.draw(x, y); break;
	case 2: game->xpm.n_two.draw(x, y); break;
	case 3: game->xpm.n_three.draw(x, y); break;
	case 4: game->xpm.n_four.draw(x, y); break;
	case 5: game->xpm.n_five.draw(x, y); break;
	case 6: game->xpm.n_six.draw(x, y); break;
	case 7: game->xpm.n_seven.draw(x, y); break;
	case 8: game->xpm.n_eight.draw(x, y); break;
	case 9: game->xpm.n_nine.draw(x, y); break;

	default: break;
	}
}

void draw_large_digit(unsigned int num, int x, int y, game_data *game) {

	switch (num) {
	case 1:	game->xpm.n_one_large.draw(x, y); break;
	case 2: game->xpm.n_two_large.draw(x, y); break;
	case 3: game->xpm.n_three_large.draw(x, y); break;
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
		temp_number = (int)floor(temp_number / 10);
		if (temp_number == 0)
			break;
	}

	for (int i = 0; i < size; ++i) {
		draw_digit(reverse_number % 10, x, y, game);
		reverse_number = (int)floor(reverse_number / 10);
		x += 10;
	}
}

void render_seq_frame(game_data *game) {

	game->bmp.game_background.draw(0, 0);
	draw_ship(&game->bmp.pix_ship_blue, &game->player1);
	draw_large_digit(game->timers.start_seq, 512, 304, game);
}

void draw_ship(Bitmap *bmp, player *p) {

	double degrees;
	mvector2d vmouse(p->pivot, p->crosshair);
	mvector2d vcannon(p->pivot, p->cannon);
	if (vmouse.magnitude() > 5)
		degrees =vmouse.angle();
	else degrees = vmouse.angle();

	mpoint2d ws_pivot = vector_translate_gfx(&p->pivot, 1024, 768);
	bmp->draw_transform((int)ws_pivot.x, (int)ws_pivot.y, degrees, 1);
}

void draw_alien(game_data *game) {
	if (game->alien.active) {
		mpoint2d ws_pivot = vector_translate_gfx(&game->alien.pivot, 1024, 768);
		mvector2d vcannon (game->alien.pivot, game->alien.cannon);
		game->bmp.alien_ship.draw_transform((int)ws_pivot.x, (int)ws_pivot.y, vcannon.angle(), 1);

		//Draw alien lasers
		for (int i = 0; i < AMMO; i++) {
			if (game->alien.lasers[i].active()) {
				mpoint2d ws_laser = vector_translate_gfx(&game->alien.lasers[i].getposition(), 1024, 768);
				game->xpm.red_laser.draw((int)ws_laser.x, (int)ws_laser.y);
			}
		}
	}

	else if (game->timers.alien_death_timer > 0) {
		mpoint2d ws_pivot = vector_translate_gfx(&game->alien.pivot, 1024, 768);
		if (game->timers.alien_death_timer > 20) {
			game->xpm.asteroid_dest1.draw ((int)ws_pivot.x, (int)ws_pivot.y);
			game->bmp.alien_score.draw ((int)ws_pivot.x + 10, (int)ws_pivot.y - 35);
		}
		else if (game->timers.alien_death_timer > 10) {
			game->xpm.asteroid_dest2.draw((int)ws_pivot.x, (int)ws_pivot.y);
			game->bmp.alien_score.draw((int)ws_pivot.x + 10, (int)ws_pivot.y - 35);
		}
		else {
			game->xpm.asteroid_dest3.draw((int)ws_pivot.x, (int)ws_pivot.y);
			game->bmp.alien_score.draw((int)ws_pivot.x + 10, (int)ws_pivot.y - 35);
		}
	}
}


void draw_ast(asteroid *ast, Bitmap *bmp) {

	mpoint2d ws_ast = vector_translate_gfx(&ast->position, 1024, 768);
	bmp->draw_transform((int)ws_ast.x, (int)ws_ast.y, ast->degrees, 1);
}

void render_frame(game_data *game) {

	game->bmp.game_background.draw(0, 0);

	mpoint2d ws_mouse = vector_translate_gfx(&game->player1.crosshair, 1024, 768);
	game->xpm.crosshair.draw((int)ws_mouse.x, (int)ws_mouse.y);
	switch (game->s_event) {

		case MAIN_THRUSTER: {
			draw_ship(&game->bmp.pix_ship_blue_bt, &game->player1);
			break;
		}
		case PORT_THRUSTER: {
			draw_ship(&game->bmp.pix_ship_blue_pt, &game->player1);
			break;
		}
		case STARBOARD_THRUSTER: {
			draw_ship(&game->bmp.pix_ship_blue_st, &game->player1);
			break;
		}
		case IDLING: {
			draw_ship(&game->bmp.pix_ship_blue, &game->player1);
			break;
		}
		default: {
			draw_ship(&game->bmp.pix_ship_blue, &game->player1);
			break;
		}

	}

	//Draw lasers
	for (int i = 0; i < AMMO; i++) {
		if (game->player1.lasers[i].active()) {
			mpoint2d ws_laser = vector_translate_gfx(&game->player1.lasers[i].getposition(), 1024, 768);
			game->xpm.blue_laser.draw((int)ws_laser.x, (int)ws_laser.y);
		}
	}


	//Draw Asteroids
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		mpoint2d ws_ast = vector_translate_gfx(&game->asteroid_field[i].position, 1024, 768);
		//Active asteroids
		if (game->asteroid_field[i].active) {
			if (game->asteroid_field[i].size == MEDIUM)
				draw_ast(&game->asteroid_field[i], &game->bmp.medium_asteroid);
			else draw_ast(&game->asteroid_field[i], &game->bmp.large_asteroid);
		}

		//Destruction animation
		else {
			Bitmap temp;
			if (game->asteroid_field[i].size == MEDIUM)
				temp = game->bmp.medium_score;
			else temp = game->bmp.large_score;

			if (game->asteroid_field[i].death_timer > 20) {
				game->xpm.asteroid_dest1.draw((int)ws_ast.x, (int)ws_ast.y);
				temp.draw((int)ws_ast.x + 10, (int)ws_ast.y - 35);
			}
			else if (game->asteroid_field[i].death_timer > 10) {
				game->xpm.asteroid_dest2.draw((int)ws_ast.x, (int)ws_ast.y);
				temp.draw((int)ws_ast.x + 10, (int)ws_ast.y - 35);
			}
			else if (game->asteroid_field[i].death_timer > 0) {
				game->xpm.asteroid_dest3.draw((int)ws_ast.x, (int)ws_ast.y);
				temp.draw((int)ws_ast.x + 10, (int)ws_ast.y - 35);
			}
		}
	}

	//Draw FPS counter
	if (game->settings.fps_counter) {
		game->bmp.fps_header.draw(912, 3);
		draw_number(game->timers.frames_per_second, 970, 10, game);
	}

	//Draw Score
	game->bmp.score_header.draw (10, 4);
	draw_number(game->player1.score, 80, 10, game);

	//Draw_HP
	if (game->player1.hp > 30)
		game->bmp.hp_header.draw (130, 4);
	else game->bmp.hp_header_low.draw(130, 4);
	draw_number(game->player1.hp, 175, 10, game);

	//Draw Teleport Header
	if (game->player1.jump_ready)
		game->bmp.teleport_ready_header.draw(220, 4);
	else game->bmp.teleport_not_ready_header.draw(220, 4);

	draw_alien(game);
}

void render_console(game_data * game) {
	game->bmp.gameconsole.draw(0, 0);
	game->console.draw_column(game->timers.timerTick);
	draw_string(5, VERT_BAR_Y_POS, game->console.get_command(), C_WHITE, game->lucida_console_med);
	std::queue <consolemessage> tempmessages = game->console.getconsole_messages();
	int messagessize = tempmessages.size();

	for (int i = 0; i < messagessize; ++i) {
		draw_string(15, 15 + (i * 14), tempmessages.front().message, tempmessages.front().messagecolor, game->lucida_console_med);
		tempmessages.pop();
	}

}

void handle_frame(game_data *game) {

	render_frame(game);
	display_frame();
	game->timers.framecounter++;
}

void handle_menu_frame(game_data *game, Bitmap *bckgrd) {

	bckgrd->draw(0, 0);

	switch (game->state) {
	case MENU: {
		for (int i = 0; i < MAX_ASTEROIDS; i++) {
			mpoint2d ws_ast = vector_translate_gfx(&game->menu_asteroid_field[i].position, 1024, 768);
			//Active asteroids
			if (game->menu_asteroid_field[i].active) {
				if (game->menu_asteroid_field[i].size == MEDIUM)
					draw_ast(&game->menu_asteroid_field[i], &game->bmp.medium_asteroid);
				else draw_ast(&game->menu_asteroid_field[i], &game->bmp.large_asteroid);
			}
		}

		game->bmp.hsbackground.draw(787, 1);
		game->bmp.playbutton.draw(380, 200);
		game->bmp.optionsbutton.draw(380, 300);
		game->bmp.quitbutton.draw(380, 400);
		for (int i = 0; i < 5; i++)
			draw_number(game->highscores[i], 900, 60 + i * 40, game);
		
		if (game->console.visible())
			render_console(game);
		

		break;
	}
	case OPTIONSMENU: {
		/* FPS Counter */
		if (game->settings.fps_counter)
			game->bmp.boxticked.draw(348, 270);
		/* Vsync */
		if (game->settings.vsync)
			game->bmp.boxticked.draw(151, 268);
		/* Resolution */
		if (game->settings.hresolution == 1920)
			game->bmp.p_arrow.draw(55, 150);
		else if (game->settings.hresolution == 1600)
			game->bmp.p_arrow.draw(62, 173);
		else if (game->settings.hresolution == 1280)
			game->bmp.p_arrow.draw(52, 196);
		else if (game->settings.hresolution == 1024)
			game->bmp.p_arrow.draw(60, 217);
		/* Display Mode */
		if (game->settings.fullscreen)
			game->bmp.p_arrow.draw(306, 147);
		else if (game->settings.fullscreennative)
			game->bmp.p_arrow.draw(222, 171);
		else if (game->settings.borderless)
			game->bmp.p_arrow.draw(300, 196);
		else game->bmp.p_arrow.draw(295, 221);
		/* FPS */
		if (!game->settings.fps)
			game->bmp.p_arrow.draw(500, 147);
		else if (game->settings.fps == 1)
			game->bmp.p_arrow.draw(555, 168);
		else if (game->settings.fps == 2)
			game->bmp.p_arrow.draw(555, 193);
		/* Music Volume */
		switch (game->settings.music_volume) {
		case 0: {
			game->bmp.slidemarker.draw(205, 382);
			break;
		}
		case 1: {
			game->bmp.slidemarker.draw(232, 382);
			break;
		}
		case 2: {
			game->bmp.slidemarker.draw(258, 382);
			break;
		}
		case 3: {
			game->bmp.slidemarker.draw(285, 382);
			break;
		}
		case 4: {
			game->bmp.slidemarker.draw(312, 382);
			break;
		}
		case 5: {
			game->bmp.slidemarker.draw(339, 382);
			break;
		}
		default: break;
		}

		/* Effects Volume */
		switch (game->settings.effects_volume) {
		case 0: {
			game->bmp.slidemarker.draw(205, 454);
			break;
		}
		case 1: {
			game->bmp.slidemarker.draw(232, 454);
			break;
		}
		case 2: {
			game->bmp.slidemarker.draw(258, 454);
			break;
		}
		case 3: {
			game->bmp.slidemarker.draw(285, 454);
			break;
		}
		case 4: {
			game->bmp.slidemarker.draw(312, 454);
			break;
		}
		case 5: {
			game->bmp.slidemarker.draw(339, 454);
			break;
		}
		default: break;
		}

		break;
	}
	default: break;

	}

	game->xpm.cursor.draw(game->SDLevent.motion.x, game->SDLevent.motion.y);

	display_frame();
}



mpoint2d vector_translate_gfx(mpoint2d *cartesian, unsigned int width, unsigned int height) {

	mpoint2d gfx_point;
	mpoint2d origin;
	origin.x = width / 2;
	origin.y = height / 2;

	gfx_point.x = origin.x + cartesian->x;
	gfx_point.y = origin.y - cartesian->y;

	return gfx_point;
}



