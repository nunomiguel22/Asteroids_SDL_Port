#include "game.h"
#include "renderer.h"


void options_menu_handler(game_data *game) {

	if (game->event == TIMER)
		handle_menu_frame(game, &game->bmp.options);

	if (game->event == MOUSE) {
		if (game->SDLevent.button.button == SDL_BUTTON_LEFT) {
			options_button_check(game);
		}
	}
}


void options_button_check(game_data* game) {

	/* Display mode options*/
	if (game->SDLevent.motion.x >= 215 && game->SDLevent.motion.x <= 410) {
		if (game->SDLevent.motion.y >= 147 && game->SDLevent.motion.y <= 167) {
			game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
			game->settings.fullscreen = true;
		}
		else if (game->SDLevent.motion.y >= 169 && game->SDLevent.motion.y <= 192) {
			game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
			game->settings.fullscreennative = true;
			game->settings.fullscreen = false;
		}
		else if (game->SDLevent.motion.y >= 194 && game->SDLevent.motion.y <= 215) {
			game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
			game->settings.fullscreennative = false;
			game->settings.fullscreen = false;
			game->settings.borderless = true;
		}
		else if (game->SDLevent.motion.y >= 217 && game->SDLevent.motion.y <= 240) {
			game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
			game->settings.fullscreennative = false;
			game->settings.fullscreen = false;
			game->settings.borderless = false;
		}

	}
	/* Resolution options*/
	if (game->SDLevent.motion.x >= 37 && game->SDLevent.motion.x <= 158) {
		if (game->SDLevent.motion.y >= 147 && game->SDLevent.motion.y <= 167) {
			game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
			game->settings.vresolution = 1080;
			game->settings.hresolution = 1920;
		}
		else if (game->SDLevent.motion.y >= 169 && game->SDLevent.motion.y <= 192) {
			game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
			game->settings.vresolution = 900;
			game->settings.hresolution = 1600;
		}
		else if (game->SDLevent.motion.y >= 194 && game->SDLevent.motion.y <= 215) {
			game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
			game->settings.vresolution = 1024;
			game->settings.hresolution = 1280;
		}
		else if (game->SDLevent.motion.y >= 217 && game->SDLevent.motion.y <= 240) {
			game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
			game->settings.vresolution = 768;
			game->settings.hresolution = 1024;
		}

	}
	/* FPS options*/
	if (game->SDLevent.motion.x >= 445 && game->SDLevent.motion.x <= 610) {
		if (game->SDLevent.motion.y >= 147 && game->SDLevent.motion.y <= 167) {
			game->settings.fps = 0;
			game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
		}
		else if (game->SDLevent.motion.y >= 169 && game->SDLevent.motion.y <= 192) {
			game->settings.fps = 1;
			game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
		}
		else if (game->SDLevent.motion.y >= 194 && game->SDLevent.motion.y <= 215) {
			game->settings.fps = 2;
			game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
		}
	}

	/* Music volume options*/
	if (game->SDLevent.motion.y >= 386 && game->SDLevent.motion.y <= 406) {
		if (game->SDLevent.motion.x >= 208 && game->SDLevent.motion.x <= 217)
			game->settings.music_volume = 0;
		else if (game->SDLevent.motion.x >= 236 && game->SDLevent.motion.x <= 245)
			game->settings.music_volume = 1;
		else if (game->SDLevent.motion.x >= 260 && game->SDLevent.motion.x <= 269)
			game->settings.music_volume = 2;
		else if (game->SDLevent.motion.x >= 287 && game->SDLevent.motion.x <= 296)
			game->settings.music_volume = 3;
		else if (game->SDLevent.motion.x >= 313 && game->SDLevent.motion.x <= 322)
			game->settings.music_volume = 4;
		else if (game->SDLevent.motion.x >= 340 && game->SDLevent.motion.x <= 349)
			game->settings.music_volume = 5;

	}

	/* Effects volume options*/
	if (game->SDLevent.motion.y >= 458 && game->SDLevent.motion.y <= 478) {
		if (game->SDLevent.motion.x >= 208 && game->SDLevent.motion.x <= 217)
			game->settings.effects_volume = 0;
		else if (game->SDLevent.motion.x >= 236 && game->SDLevent.motion.x <= 245)
			game->settings.effects_volume = 1;
		else if (game->SDLevent.motion.x >= 260 && game->SDLevent.motion.x <= 269)
			game->settings.effects_volume = 2;
		else if (game->SDLevent.motion.x >= 287 && game->SDLevent.motion.x <= 296)
			game->settings.effects_volume = 3;
		else if (game->SDLevent.motion.x >= 313 && game->SDLevent.motion.x <= 322)
			game->settings.effects_volume = 4;
		else if (game->SDLevent.motion.x >= 340 && game->SDLevent.motion.x <= 349)
			game->settings.effects_volume = 5;
	}

	/* FPS counter option*/
	if (game->SDLevent.motion.y >= 270 && game->SDLevent.motion.y <= 290 && game->SDLevent.motion.x >= 345 && game->SDLevent.motion.x <= 370) {
		game->settings.fps_counter ^= 1;
		game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
	}
	/* Vsync option*/
	if (game->SDLevent.motion.y >= 270 && game->SDLevent.motion.y <= 290 && game->SDLevent.motion.x >= 147 && game->SDLevent.motion.x <= 174) {
		game->settings.vsync ^= 1;
		game->threads.push(play_sound, std::ref(*game->sound.Beep_Short));
	}
	/* Cancel Button */
	if (game->SDLevent.motion.y >= 690 && game->SDLevent.motion.y <= 740 && game->SDLevent.motion.x >= 38 && game->SDLevent.motion.x <= 185) {
		read_game_settings(&game->settings);
		game->state = MENU;
	}
	/* Apply Button */
	if (game->SDLevent.motion.y >= 690 && game->SDLevent.motion.y <= 740 && game->SDLevent.motion.x >= 846 && game->SDLevent.motion.x <= 995) {
		change_volume(&game->sound, game->settings.effects_volume, game->settings.music_volume);
		reset_sdl(&game->settings);
		save_game_settings(&game->settings);
		game->state = MENU;
	}
	/* Reset Button */
	if (game->SDLevent.motion.y >= 690 && game->SDLevent.motion.y <= 740 && game->SDLevent.motion.x >= 235 && game->SDLevent.motion.x <= 718)
		reset_game_settings(&game->settings);
}