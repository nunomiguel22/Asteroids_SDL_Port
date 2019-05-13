#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <sstream>
#include <string>
#include <queue>

#include "renderer.h"
#include "../general/macros.h"


//#include <iostream> //DEBUG
//#include <time.h> //DEBUG



SDL_Window *window = NULL;
SDL_Renderer *renderer;
SDL_Texture *screen;
uint32_t *pixelbuffer;

const int pixel_bytes = 4;
const int vram_size = hres * vres * pixel_bytes;

uint32_t * init_sdl(game_settings *settings, console *cons) {

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::string error = SDL_GetError();
		cons->log_push("SDL failed to start, error: " + error);
		return NULL;
	}
	else cons->log_push("SDL initialized");

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

	/* Window */
	window = SDL_CreateWindow("Asteroids", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, settings->hresolution, settings->vresolution, windowFlags);
	if (window == NULL) {
		std::string error = SDL_GetError();
		cons->log_push("Failed to create window, error: " + error);
		return NULL;
	}
	else cons->log_push("Created window");
	SDL_RaiseWindow(window);
	SDL_ShowCursor(SDL_DISABLE);
	
	/* Renderer */
	renderer = SDL_CreateRenderer(window, -1, rendererFlags);
	if (renderer == NULL) {
		std::string error = SDL_GetError();
		cons->log_push("Failed to create renderer, error: " + error);
		return NULL;
	}
	else cons->log_push("Created rendender");

	/* Screen texture */
	screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, hres, vres);
	if (screen == NULL) {
		std::string error = SDL_GetError();
		cons->log_push("Failed to create screen texture, error: " + error);
		return NULL;
	}
	else cons->log_push("Created screen texture");


	pixelbuffer = (uint32_t *)malloc(vram_size);
	if (pixelbuffer == NULL) {
		cons->log_push("Failed to create pixel buffer");
		return NULL;
	}
	else cons->log_push("Created pixel buffer");
	memset(pixelbuffer, 0, vram_size);

	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
		std::string error = SDL_GetError();
		cons->log_push("Failed to initialize audio API, error: " + error);
		return NULL;
	}
	else cons->log_push("Audio API initialized");

	if (TTF_Init() < 0){
		std::string error = SDL_GetError();
		cons->log_push("Failed to initialize TTF font API, error: " + error);
		return NULL;
	}
	else cons->log_push("TTF font API initialized");

	if (SDLNet_Init() < 0) {
		std::string error = SDLNet_GetError();
		cons->log_push("Failed to initialize network API, error: " + error);
		return NULL;
	}
	else cons->log_push("Network API initialized");

	cons->log_push(" ");
	
	return pixelbuffer;
}

void  exit_sdl(game_data *game) {

	TTF_Quit();

	Mix_CloseAudio();

	if (pixelbuffer != NULL)
		free(pixelbuffer);
	if (window != NULL)
		SDL_DestroyWindow(window);
	if (renderer != NULL)
		SDL_DestroyRenderer(renderer);
	if (screen != NULL)
		SDL_DestroyTexture(screen);

	SDLNet_Quit();

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

mpoint2d vector_translate_gfx(mpoint2d *cartesian, unsigned int width, unsigned int height) {

	mpoint2d gfx_point;
	mpoint2d origin;
	origin.x = (float)(width / 2);
	origin.y = (float)(height / 2);

	gfx_point.x = origin.x + cartesian->x;
	gfx_point.y = origin.y - cartesian->y;

	return gfx_point;
}

int draw_pixel(int x, int y, uint32_t color) {

	if (x > hres - 1 || y > vres - 1 || x < 0 || y < 0)
		return 1;

	uint8_t alpha = color & 0x000000FF;
	if (alpha < 77)
		return 0;

	uint32_t *v_address = pixelbuffer;
	
	/* Move pointer to pixel location */
	v_address += (((hres) * y) + x);

	uint32_t fg_color = color >> 8;
	uint32_t bg_color = *v_address >> 8;

	/* If the pixel is blank no need to blend, if foreground color is black there is no need to write anything to the buffer */
	if (!fg_color && !bg_color)
		return 0;

	/* Blend colors, no need to calculate blend if alpha is minimum or maximum */

	if (alpha == 0xFF) {
		*v_address = color >> 8;
		return 0;
	}

	/* Blending */
	
	uint8_t fg_blue = fg_color;
	uint8_t fg_green = fg_color >> 8;
	uint8_t fg_red = fg_color >> 16;

	uint8_t bg_red = bg_color;
	uint8_t bg_green = bg_color >> 8;
	uint8_t bg_blue = bg_color >> 16;
	
	uint32_t final_blue = (alpha * (fg_blue - bg_blue) >> 8 )+ bg_blue;
	uint32_t final_green = (alpha * (fg_green - bg_green) >> 8 )+ bg_green;
	uint32_t final_red = (alpha * (fg_red - bg_red) >> 8 )+ bg_red;

	uint32_t final_color = final_blue | (final_green << 8) | (final_red << 16);
	
	/* Write final color to buffer */
	*v_address = final_color;

	return 0;
}

void display_frame() {

	SDL_UpdateTexture(screen, NULL, pixelbuffer, hres * 4);
	SDL_RenderCopy(renderer, screen, NULL, NULL);
	SDL_RenderPresent(renderer);

	memset(pixelbuffer, 0, vram_size);
}

void render_hp_bar(player *p, int x, int y) {

	int bar_length = (int)(p->hp * 1.53);
	uint32_t color;
	if (p->hp > 70)
		color = C_GREEN;
	else if (p->hp >= 40)
		color = C_YELLOW;
	else color = C_RED;

	for (int i = 0; i < 21; ++i)
		for (int j = 0; j < bar_length; ++j) {
			draw_pixel(x + j, y + i, color);
		}
}

void render_mp_hp_bars(player *p, int x, int y) {

	int bar_length = (int)(p->hp * 2.18);
	uint32_t color;
	if (p->hp > 70)
		color = C_GREEN;
	else if (p->hp >= 40)
		color = C_YELLOW;
	else color = C_RED;

	for (int i = 0; i < 13; ++i)
		for (int j = 0; j < bar_length; ++j) {
			draw_pixel(x + j, y + i, color);
		}
}

void render_laser(player *p, Bitmap *laser, weapon *l) {
	mpoint2d laser_pos = l->get_position();
	mpoint2d ws_laser = vector_translate_gfx(&laser_pos, hres, vres);
	laser->draw_transform((int)ws_laser.x, (int)ws_laser.y, l->get_angle(), BMP_CENTER);
}

void draw_ship(Bitmap *bmp, player *p) {

	float degrees;
	mvector2d vmouse(p->pivot, p->crosshair);
	mvector2d vcannon(p->pivot, p->cannon);
	if (vmouse.magnitude() > 5)
		degrees =vmouse.angle();
	else degrees = vmouse.angle();

	mpoint2d ws_pivot = vector_translate_gfx(&p->pivot, 1024, 768);
	bmp->draw_transform((int)ws_pivot.x, (int)ws_pivot.y, degrees, BMP_CENTER);
}

void draw_alien(game_data *game) {
	static int expl_frame = 0;
	if (game->alien.status & BIT(0)) {
		mpoint2d ws_pivot = vector_translate_gfx(&game->alien.pivot, 1024, 768);
		mvector2d vcannon (game->alien.pivot, game->alien.cannon);
		game->bmp.alien_ship.draw_transform((int)ws_pivot.x, (int)ws_pivot.y, vcannon.angle() - 90, BMP_CENTER);

		//Draw alien lasers
		for (int i = 0; i < AMMO; i++) {
			if (game->alien.lasers[i].is_active()) {
				render_laser(&game->alien, &game->bmp.laser_red, &game->alien.lasers[i]);
			}
		}
	}

	else if (game->timers.alien_death_timer > 0) {
		mpoint2d ws_pivot = vector_translate_gfx(&game->alien.pivot, 1024, 768);
		game->bmp.alien_score.draw((int)ws_pivot.x + 10, (int)ws_pivot.y - 35);

		if (expl_frame > 9)
			expl_frame = 0;
		
		game->bmp.ship_expl[expl_frame].draw_transform((int)ws_pivot.x, (int)ws_pivot.y, 0, BMP_CENTER);

		/*
			NEEDS TO BE MOVED TO INCREMENT TIMERS LATER, ANIMATION ON REPEAT WITH HIGH FRAMES
			
		*/
		if (game->timers.alien_death_timer % 3 == 0) 
			++expl_frame;
	}
}


void draw_ast(asteroid *ast, Bitmap *bmp) {

	mpoint2d ws_ast = vector_translate_gfx(&ast->position, hres, vres);
	bmp->draw_transform((int)ws_ast.x, (int)ws_ast.y, (float)ast->degrees, BMP_CENTER);
}

void render_crosshair(Bitmap *delta, player *player1) {
	mvector2d vmouse(player1->pivot, player1->crosshair);
	mpoint2d ws_mouse = vector_translate_gfx(&player1->crosshair, hres, vres);
	delta->draw_transform((int)ws_mouse.x, (int)ws_mouse.y, vmouse.angle() - 90, BMP_CENTER);
}

void show_splash(game_data *game) {
	game->bmp.splash.draw(0, 0);
	game->bmp.pix_ship_blue.draw_transform(200, 400, 30, BMP_CENTER);
	display_frame();
	SDL_Delay(3000);
	display_frame();
	game->bmp.menubackground.draw(0, 0);
	game->bmp.cursor.draw(hres / 2, vres / 2);
	display_frame();
}

void render_frame(game_data *game) {
	

	game->bmp.game_background.draw_background(0, 0);

	
	//If ship is teleporting draw effect
	if (game->player1.status & BIT(5))
		draw_ship(&game->bmp.pix_ship_blue_tele, &game->player1);

	switch (game->player1.s_event) {

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
	for (int i = 0; i < AMMO; ++i) {
		if (game->player1.lasers[i].is_active()) {
			render_laser(&game->player1, &game->bmp.laser_blue, &game->player1.lasers[i]);
		}
	}


	//Draw Asteroids
	for (int i = 0; i < MAX_ASTEROIDS; ++i) {
		mpoint2d ws_ast = vector_translate_gfx(&game->asteroid_field[i].position, hres, vres);
		
		//Active asteroids
		if (game->asteroid_field[i].active) {
			
			if (game->asteroid_field[i].size == MEDIUM)
				draw_ast(&game->asteroid_field[i], &game->bmp.medium_asteroid);
			else draw_ast(&game->asteroid_field[i], &game->bmp.large_asteroid);
			
		}
		//Destruction animation
		else {
		
			Bitmap *temp;
			if (game->asteroid_field[i].size == MEDIUM)
				temp = &game->bmp.medium_score;
			else temp = &game->bmp.large_score;

			if (game->asteroid_field[i].death_timer > 0) {
				temp->draw((int)ws_ast.x + 10, (int)ws_ast.y - 35);

				if (game->asteroid_field[i].death_frame > 9)
					game->asteroid_field[i].death_frame = 0;

				game->bmp.ast_dest[game->asteroid_field[i].death_frame].draw_transform((int)ws_ast.x, (int)ws_ast.y, (float)game->asteroid_field[i].degrees, BMP_CENTER);

			}
		}
	}

	if (game->timers.hitreg_timer > 0)
		render_crosshair(&game->bmp.delta_hit, &game->player1);
	else render_crosshair(&game->bmp.delta, &game->player1);

	game->bmp.ui_status.draw(10, 600);

	//Draw FPS counter
	if (game->settings.fps_counter) {
		game->ttf_fonts.lucida_console_med.render_string(10, 10, "fps ", C_GREEN);
		game->ttf_fonts.lucida_console_med.render_number((float)game->timers.frames_per_second, 35, 10, C_GREEN);
	}

	//Draw Score
	game->ttf_fonts.copperplategothicbold.render_number((float)game->player1.score, 195, 710, C_WHITE);

	//Draw_HP
	render_hp_bar(&game->player1, 198, 634);

	//Draw round
	game->ttf_fonts.copperplategothicbold.render_number((float)game->player1.round - 4, 195, 686, C_WHITE);

	//Draw Teleport status
	if (game->player1.status & BIT(6))
		game->ttf_fonts.copperplategothicbold.render_string(195, 662, "ready", C_GREEN);
	else game->ttf_fonts.copperplategothicbold.render_string(195, 662, "down", C_RED);

	draw_alien(game);

	if (game->console.is_open())
		render_console(game);
}

void render_mp_frame(game_data *game) {

	static bool cnt_msg = false;
	static unsigned int cnt_time = 0;
	static int expl_frame = 0;

	game->bmp.mp_background.draw_background(0, 0);



	if (game->player1.status & BIT(0)) {
		if (game->player1.status & BIT(5))
			draw_ship(&game->bmp.pix_ship_blue_tele, &game->player1);

		switch (game->player1.s_event) {

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
	}
	else if (game->timers.player1_death_timer > 0) {
		mpoint2d ws_pivot = vector_translate_gfx(&game->player1.pivot, 1024, 768);

		if (expl_frame > 9)
			expl_frame = 0;

		game->bmp.ship_expl[expl_frame].draw_transform((int)ws_pivot.x, (int)ws_pivot.y, 0, BMP_CENTER);

		if (game->timers.player1_death_timer % 3 == 0)
			++expl_frame;
	}


	if (game->player2.status & BIT(0)) {

		//If ship is teleporting draw effect
		if (game->player2.status & BIT(5))
			draw_ship(&game->bmp.pix_ship_alt_tele, &game->player2);

		switch (game->player2.s_event) {

			case MAIN_THRUSTER: {
				draw_ship(&game->bmp.pix_ship_alt_bt, &game->player2);
				break;
			}
			case PORT_THRUSTER: {
				draw_ship(&game->bmp.pix_ship_alt_pt, &game->player2);
				break;
			}
			case STARBOARD_THRUSTER: {
				draw_ship(&game->bmp.pix_ship_alt_st, &game->player2);
				break;
			}
			case IDLING: {
				draw_ship(&game->bmp.pix_ship_alt, &game->player2);
				break;
			}
			default: {
				draw_ship(&game->bmp.pix_ship_alt, &game->player2);
				break;
			}
		}
	}
	else if (game->timers.player2_death_timer > 0) {
		mpoint2d ws_pivot = vector_translate_gfx(&game->player2.pivot, 1024, 768);

		if (expl_frame > 9)
			expl_frame = 0;

		game->bmp.ship_expl[expl_frame].draw_transform((int)ws_pivot.x, (int)ws_pivot.y, 0, BMP_CENTER);

		if (game->timers.player2_death_timer % 3 == 0)
			++expl_frame;
	}


	//Draw player 1 lasers
	for (int i = 0; i < AMMO; ++i) {
		if (game->player1.lasers[i].is_active()) {
			render_laser(&game->player1, &game->bmp.laser_blue, &game->player1.lasers[i]);
		}
	}

	//Draw player 2 lasers
	if (game->player2.status & BIT(0)) {
		for (int i = 0; i < AMMO; ++i) {
			if (game->player2.lasers[i].is_active()) {
				render_laser(&game->player2, &game->bmp.laser_red, &game->player2.lasers[i]);
			}
		}
	}

	if (game->timers.hitreg_timer > 0)
		render_crosshair(&game->bmp.delta_hit, &game->player1);
	else render_crosshair(&game->bmp.delta, &game->player1);

	//Draw FPS counter
	if (game->settings.fps_counter) {
		game->ttf_fonts.lucida_console_med.render_string(10, 10, "fps ", C_GREEN);
		game->ttf_fonts.lucida_console_med.render_number((float)game->timers.frames_per_second, 35, 10, C_GREEN);
	}


	if (game->state == MP_ROUND) {
		game->bmp.mp_nameplate.draw(120, 20);
		game->ttf_fonts.copperplategothicbold_massive.render_string(140, 20, game->player1.name, C_WHITE);
		game->bmp.mp_scoreplate.draw(350, 20);
		game->ttf_fonts.copperplategothicbold_massive.render_number(game->player1.mp_round, 355, 20, C_WHITE);
		game->bmp.mp_hpbaroutline.draw(120, 80);
		render_mp_hp_bars(&game->player1, 125, 85);

		game->ttf_fonts.copperplategothicbold_massive.render_string(455, 20, "VS", C_WHITE);

		game->bmp.mp_scoreplate.draw(578, 20);
		game->ttf_fonts.copperplategothicbold_massive.render_number(game->player2.mp_round, 583, 20, C_WHITE);
		game->bmp.mp_nameplate.draw(632, 20);
		game->ttf_fonts.copperplategothicbold_massive.render_string(642, 20, game->player2.name, C_WHITE);
		game->bmp.mp_hpbaroutline.draw(622, 80);
		render_mp_hp_bars(&game->player2, 627, 85);
	}

	if (!cnt_time && !cnt_msg && game->connection.is_connected()) {
			cnt_time = game->timers.timerTick + 120;
			cnt_msg = true;
	}

	if (cnt_msg) {
		if (game->timers.timerTick < cnt_time)
			game->ttf_fonts.lucida_console_med.render_string(10, 50, game->player2.name + " joined ", C_WHITE);
		else cnt_msg = false;
	}

	if (game->state == MP_WARMUP) {
		if (cnt_time) {
			if (game->player1.status & BIT(1)) {
				game->ttf_fonts.copperplategothicbold.render_string(420, 150, "Press F3 to ", C_WHITE);
				game->ttf_fonts.copperplategothicbold.render_string(520, 150, "unready", C_RED);
			}
			else {
				game->ttf_fonts.copperplategothicbold.render_string(420, 150, "Press F3 to ", C_WHITE);
				game->ttf_fonts.copperplategothicbold.render_string(520, 150, "ready up", C_GREEN);
			}
		}
		else game->ttf_fonts.copperplategothicbold.render_string(420, 150, "Waiting for enemy", C_WHITE);
	}



	if (game->console.is_open())
		render_console(game);
}

void handle_frame_wrapper(int id, game_data &game) {
	handle_frame(&game);
}

void render_seq_frame(game_data *game) {

	game->bmp.game_background.draw(0, 0);
	draw_ship(&game->bmp.pix_ship_blue, &game->player1);
	game->ttf_fonts.copperplategothicbold_massive.render_number((float)game->timers.start_seq, 512, 304, C_WHITE);
}

void render_mp_seq_frame(game_data *game) {
	game->bmp.mp_background.draw(0, 0);

	game->bmp.mp_nameplate.draw(120, 20);
	game->ttf_fonts.copperplategothicbold_massive.render_string(140, 20, game->player1.name, C_WHITE);
	game->bmp.mp_scoreplate.draw(350, 20);
	game->ttf_fonts.copperplategothicbold_massive.render_number(game->player1.mp_round, 355, 20, C_WHITE);
	game->bmp.mp_hpbaroutline.draw(120, 80);
	render_mp_hp_bars(&game->player1, 125, 85);

	game->ttf_fonts.copperplategothicbold_massive.render_string(455, 20, "VS", C_WHITE);

	game->bmp.mp_scoreplate.draw(578, 20);
	game->ttf_fonts.copperplategothicbold_massive.render_number(game->player2.mp_round, 583, 20, C_WHITE);
	game->bmp.mp_nameplate.draw(632, 20);
	game->ttf_fonts.copperplategothicbold_massive.render_string(642, 20, game->player2.name, C_WHITE);
	game->bmp.mp_hpbaroutline.draw(622, 80);
	render_mp_hp_bars(&game->player2, 627, 85);

	draw_ship(&game->bmp.pix_ship_blue, &game->player1);
	draw_ship(&game->bmp.pix_ship_alt, &game->player2);
	game->ttf_fonts.copperplategothicbold_massive.render_number((float)game->timers.start_seq, 512, 304, C_WHITE);
}

void render_console(game_data * game) {
	game->bmp.gameconsole.draw(0, 0);
	game->console.vertbar_render(game->timers.timerTick);
	game->ttf_fonts.lucida_console_med.render_string(5, VERT_BAR_Y_POS, game->console.input(), C_WHITE);

	std::queue <consolemessage> tempmessages = game->console.get_console_messages();
	
	int messagessize = tempmessages.size();

	for (int i = 0; i < messagessize; ++i) {
		game->ttf_fonts.lucida_console_med.render_string(15, 15 + (i * 14), tempmessages.front().message, tempmessages.front().messagecolor);
		tempmessages.pop();
	}
}

void handle_frame(game_data *game) {

	render_frame(game);
	display_frame();
	game->timers.framecounter++;
}

void handle_mp_frame(game_data *game) {
	render_mp_frame(game);
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

		Bitmap *playb = &game->bmp.nm_button;
		uint32_t playb_color = C_LIGHTGRAY;
		Bitmap *hostb = &game->bmp.nm_button;
		uint32_t hostb_color = C_LIGHTGRAY;
		Bitmap *cntb = &game->bmp.nm_button;
		uint32_t cntb_color = C_LIGHTGRAY;
		Bitmap *optb = &game->bmp.nm_button;
		uint32_t optb_color = C_LIGHTGRAY;
		Bitmap *gitb = &game->bmp.nm_button;
		uint32_t gitb_color = C_LIGHTGRAY;
		Bitmap *quitb = &game->bmp.nm_button;
		uint32_t quitb_color = C_LIGHTGRAY;

		if (game->SDLevent.motion.x > 395 && game->SDLevent.motion.x < 573) {
			if (game->SDLevent.motion.y > 200 && game->SDLevent.motion.y < 228) {
				playb = &game->bmp.hl_button;
				playb_color = C_WHITE;
			}
			else if (game->SDLevent.motion.y > 240 && game->SDLevent.motion.y < 268) {
				hostb = &game->bmp.hl_button;
				hostb_color = C_WHITE;
			}
			else if (game->SDLevent.motion.y > 280 && game->SDLevent.motion.y < 308) {
				cntb = &game->bmp.hl_button;
				cntb_color = C_WHITE;
			}
			else if (game->SDLevent.motion.y > 320 && game->SDLevent.motion.y < 348) {
				optb = &game->bmp.hl_button;
				optb_color = C_WHITE;
			}
			else if (game->SDLevent.motion.y > 360 && game->SDLevent.motion.y < 388) {
				gitb = &game->bmp.hl_button;
				gitb_color = C_WHITE;
			}
			else if (game->SDLevent.motion.y > 400 && game->SDLevent.motion.y < 428) {
				quitb = &game->bmp.hl_button;
				quitb_color = C_WHITE;
			}
		}

		playb->draw(395, 200);
		game->ttf_fonts.copperplategothicbold.render_string(460, 205, "Play", playb_color);

		hostb->draw(395, 240);
		game->ttf_fonts.copperplategothicbold.render_string(440, 245, "Host game", hostb_color);

		cntb->draw(395, 280);
		game->ttf_fonts.copperplategothicbold.render_string(413, 285, "Connect to game", cntb_color);

		optb->draw(395, 320);
		game->ttf_fonts.copperplategothicbold.render_string(445, 325, "Settings", optb_color);

		gitb->draw(395, 360);
		game->ttf_fonts.copperplategothicbold.render_string(428, 365, "Github repo", gitb_color);

		quitb->draw(395, 400);
		game->ttf_fonts.copperplategothicbold.render_string(460, 405, "Exit", quitb_color);


		for (int i = 0; i < 5; i++)
			game->ttf_fonts.copperplategothicbold.render_number((float)game->highscores[i], 900, 60 + i * 40, C_WHITE);
		
		if (game->console.is_open())
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

	game->bmp.cursor.draw(game->SDLevent.motion.x, game->SDLevent.motion.y);

	display_frame();
}

