#pragma once
#include <string>
#include <SDL.h>
#include <queue>


typedef struct {
	std::string message;
	uint32_t messagecolor;

}consolemessage;

class console {
	bool vis;
	unsigned int columnposition;
	std::string user_command;
	std::queue <consolemessage> console_messages;


public:
	console();


	bool visible();
	std::string get_command() { return user_command; }
	int getcolumnposition() { return columnposition; }
	std::queue<consolemessage> getconsole_messages() { return console_messages; }

	void setvisibility(bool visibility);

	void draw_column(unsigned int timerTick);
	void inc_colpos();
	void dec_colpos();
	void reset_colpos();

	std::string console_input_handler(SDL_Event &evnt);

	void write_message(std::string message, uint32_t color);
	
};

