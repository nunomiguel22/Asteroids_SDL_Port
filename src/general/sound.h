#pragma once
#include "SDL_mixer.h"

typedef struct {
	Mix_Music *galaxia;
	Mix_Music *kawaii;
	Mix_Chunk *laser;
	Mix_Chunk *pop;
	Mix_Chunk *Beep_Short;
	

}sound_data;

void load_sounds(sound_data *sounds, int eff_vol, int mus_vol);

void play_sound(int id, Mix_Chunk &sound);
void play_music(int id, Mix_Music &music);
void stop_music(int id);
void free_sounds(sound_data *sounds);
void change_volume(sound_data *sounds, int eff_vol, int mus_vol);
