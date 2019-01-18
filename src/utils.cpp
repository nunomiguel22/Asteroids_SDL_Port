#include <fstream>


#include "utils.h"
#pragma warning(disable:4996) //Ignores Visual Studio's f_open error, comment if using another compiler

void read_game_settings(game_settings *settings) {

	std::ifstream file;

	file.open("config.txt");

	if (!file.is_open()) {
		reset_game_settings(settings);
		return;
	}

	file.ignore(22);
	file >> settings->hresolution;
	file.ignore(20);
	file >> settings->vresolution;
	file.ignore(11);
	file >> settings->fullscreen;
	file.ignore(28);
	file >> settings->fullscreennative;
	file.ignore(11);
	file >> settings->borderless;
	file.ignore(7);
	file >> settings->vsync;
	file.ignore(4);
	file >> settings->fps;
	file.ignore(12);
	file >> settings->fps_counter;
	file.ignore(13);
	file >> settings->music_volume;
	file.ignore(16);
	file >> settings->effects_volume;

	file.close();
}

void reset_game_settings(game_settings *settings) {

	settings->vresolution = 768;
	settings->hresolution = 1024;
	settings->fullscreen = false;
	settings->fullscreennative = false;
	settings->vsync = false;
	settings->borderless = false;
	settings->fps_counter = true;
	settings->music_volume = 2;
	settings->effects_volume = 3;
	settings->fps = 1;
}

void save_game_settings(game_settings *settings) {

	std::ofstream file;

	file.open("config.txt");

	file << "Horizontal_resolution " << settings->hresolution << std::endl;
	file << "Vertical_resolution " << settings->vresolution << std::endl;
	file << "Fullscreen " << settings->fullscreen << std::endl;
	file << "Fullscreen_native_stretched " << settings->fullscreennative << std::endl;
	file << "Borderless " << settings->borderless << std::endl;
	file << "V-sync " << settings->vsync << std::endl;
	file << "fps " << settings->fps << std::endl;
	file << "fps_counter " << settings->fps_counter << std::endl;
	file << "music_volume " << settings->music_volume << std::endl;
	file << "effects_volume " << settings->effects_volume;

	file.close();
}

/* HIGHSCORES */

int load_highscores(unsigned int highscores[]) {

	/* Opens "highscores.txt" and loads highscores to the highscore array */
	FILE *fptr;
	fptr = fopen("highscores.txt", "r");
	if (fptr == NULL)
		return 0;

	for (int i = 0; i < 5; i++)
		fscanf(fptr, "%d", &highscores[i]);

	return 1;
}

void save_highscores(unsigned int highscores[]) {

	/* Opens "highscores.txt" and saves highscores to the file */
	FILE *fptr;

	fptr = fopen("highscores.txt", "w");
	for (int i = 0; i < 5; i++)
		fprintf(fptr, "%d\n", highscores[i]);

	fclose(fptr);
}

int verify_highscores(unsigned int highscores[], player *player1) {

	/* Check if score is a highscore */
	if (player1->score > highscores[4]) {
		highscores[4] = player1->score;

		/* Sort highscore array by highest to loweat if a new highscore is found */
		for (int i = 0; i < 5; i++) {
			for (int j = i + 1; j < 5; j++) {
				if (highscores[i] < highscores[j]) {
					unsigned int tempvar = highscores[i];
					highscores[i] = highscores[j];
					highscores[j] = tempvar;
				}
			}
		}
		/* Save highscores if a new highscore is found */
		save_highscores(highscores);
		return 1;
	}

	return 0;
}
