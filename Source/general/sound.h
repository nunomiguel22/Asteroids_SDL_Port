#pragma once
#include "SDL2/SDL_mixer.h"
#include "console.h"

typedef struct {
	Mix_Music *galaxia;
	Mix_Music *kawaii;
	Mix_Chunk *laser;
	Mix_Chunk *thrust;
	Mix_Chunk *teleport;
	Mix_Chunk *alien_spawn;
	Mix_Chunk *alien_laser;
	Mix_Chunk *ship_expl;
	Mix_Chunk *pop;
	Mix_Chunk *Beep_Short;
	Mix_Chunk *cdowna;
	Mix_Chunk *cdownb;
	Mix_Chunk *menu_tick;
	

}sound_data;

int load_sounds(sound_data *sounds, int eff_vol, int mus_vol, console *cons);

void play_sound(int id, Mix_Chunk &sound);
void play_music(int id, Mix_Music &music);
void stop_music(int id);
void free_sounds(sound_data *sounds);
void change_volume(sound_data *sounds, int eff_vol, int mus_vol);
