#include "scard.h"

void load_sounds(sound_data *sounds, int eff_vol, int mus_vol) {

	sounds->laser = Mix_LoadWAV("sounds/Laser_Gun.wav");
	sounds->pop = Mix_LoadWAV("sounds/Pop.wav");
	sounds->kawaii = Mix_LoadMUS("sounds/Kawaii.wav");
	Mix_VolumeChunk(sounds->laser, (MIX_MAX_VOLUME / 10) * eff_vol);
	Mix_VolumeChunk(sounds->pop, (MIX_MAX_VOLUME / 10) * eff_vol);
	Mix_VolumeMusic((MIX_MAX_VOLUME / 20) * mus_vol);
	
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

	Mix_FreeChunk(sounds->laser);
	Mix_FreeChunk(sounds->pop);
	Mix_FreeMusic(sounds->kawaii);
}

void change_volume(sound_data *sounds, int eff_vol, int mus_vol) {
	Mix_VolumeChunk(sounds->laser, (MIX_MAX_VOLUME / 10) * eff_vol);
	Mix_VolumeChunk(sounds->pop, (MIX_MAX_VOLUME / 10) * eff_vol);
	Mix_VolumeMusic((MIX_MAX_VOLUME / 20) * mus_vol);
}