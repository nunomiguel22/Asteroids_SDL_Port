#include "sound.h"
#include "macros.h"

int load_sounds(sound_data *sounds, int eff_vol, int mus_vol, console *cons) {

	cons->write_to_log("LOADING SOUND FILES");
	std::string file;

	/* Effects */

	file = FILE_SOUNDS_PATH "Laser_Gun.wav";
	sounds->laser = Mix_LoadWAV(file.c_str());
	if (sounds->laser == NULL) {
		std::string error = SDL_GetError();
		cons->write_to_log("Couldn't load \"" + file + "\", error: " + error);
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_SOUNDS_PATH "Pop.wav";
	sounds->pop = Mix_LoadWAV(file.c_str());
	if (sounds->pop == NULL) {
		std::string error = SDL_GetError();
		cons->write_to_log("Couldn't load \"" + file + "\", error: " + error);
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_SOUNDS_PATH "Beep_Short.wav";
	sounds->Beep_Short = Mix_LoadWAV(file.c_str());
	if (sounds->Beep_Short == NULL) {
		std::string error = SDL_GetError();
		cons->write_to_log("Couldn't load \"" + file + "\", error: " + error);
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");
	
	file = FILE_SOUNDS_PATH "thruster.wav";
	sounds->thrust = Mix_LoadWAV(file.c_str());
	if (sounds->thrust == NULL) {
		std::string error = SDL_GetError();
		cons->write_to_log("Couldn't load \"" + file + "\", error: " + error);
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_SOUNDS_PATH "teleport.wav";
	sounds->teleport = Mix_LoadWAV(file.c_str());
	if (sounds->teleport == NULL) {
		std::string error = SDL_GetError();
		cons->write_to_log("Couldn't load \"" + file + "\", error: " + error);
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_SOUNDS_PATH "alien_spawn.wav";
	sounds->alien_spawn = Mix_LoadWAV(file.c_str());
	if (sounds->alien_spawn == NULL) {
		std::string error = SDL_GetError();
		cons->write_to_log("Couldn't load \"" + file + "\", error: " + error);
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_SOUNDS_PATH "ship_explosion.wav";
	sounds->ship_expl = Mix_LoadWAV(file.c_str());
	if (sounds->ship_expl == NULL) {
		std::string error = SDL_GetError();
		cons->write_to_log("Couldn't load \"" + file + "\", error: " + error);
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_SOUNDS_PATH "alien_laser.wav";
	sounds->alien_laser = Mix_LoadWAV(file.c_str());
	if (sounds->alien_laser == NULL) {
		std::string error = SDL_GetError();
		cons->write_to_log("Couldn't load \"" + file + "\", error: " + error);
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_SOUNDS_PATH "cdowna.wav";
	sounds->cdowna = Mix_LoadWAV(file.c_str());
	if (sounds->cdowna == NULL) {
		std::string error = SDL_GetError();
		cons->write_to_log("Couldn't load \"" + file + "\", error: " + error);
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_SOUNDS_PATH "cdownb.wav";
	sounds->cdownb = Mix_LoadWAV(file.c_str());
	if (sounds->cdownb == NULL) {
		std::string error = SDL_GetError();
		cons->write_to_log("Couldn't load \"" + file + "\", error: " + error);
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	/* Music */

	file = FILE_SOUNDS_PATH "Kawaii.wav";
	sounds->kawaii = Mix_LoadMUS(file.c_str());
	if (sounds->kawaii == NULL) {
		std::string error = SDL_GetError();
		cons->write_to_log("Couldn't load \"" + file + "\", error: " + error);
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	file = FILE_SOUNDS_PATH "Galaxia.wav";
	sounds->galaxia = Mix_LoadMUS(file.c_str());
	if (sounds->galaxia == NULL) {
		std::string error = SDL_GetError();
		cons->write_to_log("Couldn't load \"" + file + "\", error: " + error);
		return 1;
	}
	else cons->write_to_log("Loaded \"" + file + "\"");

	cons->write_to_log("SOUND FILES LOADED");
	cons->write_to_log(" ");

	Mix_VolumeChunk(sounds->Beep_Short, (MIX_MAX_VOLUME >> 3) * eff_vol);
	Mix_VolumeChunk(sounds->laser, (MIX_MAX_VOLUME >> 3) * eff_vol);
	Mix_VolumeChunk(sounds->alien_laser, (MIX_MAX_VOLUME >> 3) * eff_vol);
	Mix_VolumeChunk(sounds->pop, (MIX_MAX_VOLUME >> 3) * eff_vol);
	Mix_VolumeChunk(sounds->teleport, (MIX_MAX_VOLUME >> 3) * eff_vol);
	Mix_VolumeChunk(sounds->alien_spawn, (MIX_MAX_VOLUME >> 3) * eff_vol);
	Mix_VolumeChunk(sounds->thrust, (MIX_MAX_VOLUME >> 6) * eff_vol);
	Mix_VolumeChunk(sounds->ship_expl, (MIX_MAX_VOLUME * eff_vol));
	Mix_VolumeChunk(sounds->cdowna, (MIX_MAX_VOLUME >> 3) * eff_vol);
	Mix_VolumeChunk(sounds->cdownb, (MIX_MAX_VOLUME >> 3) * eff_vol);

	return 0;
}

void play_sound(int id, Mix_Chunk &sound) {

	Mix_PlayChannel(-1, &sound, 0);
}

void play_music(int id, Mix_Music &music) {
	
	Mix_FadeInMusic(&music, -1, 2000);
}

void stop_music(int id) {
	Mix_FadeOutMusic(2000);
	Mix_RewindMusic();
}

void free_sounds(sound_data *sounds) {

	Mix_FreeChunk(sounds->Beep_Short);
	Mix_FreeChunk(sounds->laser);
	Mix_FreeChunk(sounds->pop);
	Mix_FreeChunk(sounds->alien_laser);
	Mix_FreeChunk(sounds->teleport);
	Mix_FreeChunk(sounds->alien_spawn);
	Mix_FreeChunk(sounds->thrust);
	Mix_FreeChunk(sounds->ship_expl);
	Mix_FreeChunk(sounds->cdowna);
	Mix_FreeChunk(sounds->cdownb);

	Mix_FreeMusic(sounds->kawaii);
	Mix_FreeMusic(sounds->galaxia);
}

void change_volume(sound_data *sounds, int eff_vol, int mus_vol) {
	Mix_VolumeChunk(sounds->Beep_Short, (MIX_MAX_VOLUME / 10) * eff_vol);
	Mix_VolumeChunk(sounds->laser, (MIX_MAX_VOLUME / 10) * eff_vol);
	Mix_VolumeChunk(sounds->pop, (MIX_MAX_VOLUME / 10) * eff_vol);
	Mix_VolumeMusic((MIX_MAX_VOLUME / 20) * mus_vol);
}