#pragma once
#include <string>
#include <SDL.h>
#include <queue>

class console {
	bool vis;
	unsigned int columnposition;
	std::string user_command;
	std::queue <std::string> console_messages;


public:
	console() { columnposition = 5; console_messages.push("Asteroids by Nuno Marques"); console_messages.push("Console version 0.3");
	}

	bool visible();
	std::string get_command() { return user_command; }
	int getcolumnposition() { return columnposition; }
	std::queue<std::string> getconsole_messages() { return console_messages; }

	void setvisibility(bool visibility);

	void draw_column(unsigned int timerTick);
	void inc_colpos();
	void dec_colpos();
	void reset_colpos();

	int console_input_handler(SDL_Event &evnt);

	void write_message(std::string message);
	
};

