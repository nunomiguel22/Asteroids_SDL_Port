#pragma once
#include <string>
#include <SDL.h>
#include <queue>
#include <map>

typedef enum {	con_not_defined, con_init_server, con_init_client, con_connect, con_quit, con_clear } console_commands;

typedef struct {
	std::string message;
	uint32_t messagecolor;
}consolemessage;



class console {
	bool vis;
	unsigned int vertbarposition;
	std::string user_command;
	std::queue <consolemessage> console_messages;
	std::map <std::string, console_commands> command_map;

public:
	console();
	
	bool visible();
	std::string get_command() { return user_command; }
	int getvertbarposition() { return vertbarposition; }
	std::queue<consolemessage> getconsole_messages() { return console_messages; }
	std::map <std::string, console_commands>* get_command_map() { return &command_map; }

	void setvisibility(bool visibility);

	void draw_column(unsigned int timerTick);
	void inc_colpos();
	void dec_colpos();
	void reset_colpos();

	std::string console_input_handler(SDL_Event &evnt);
	void write_message(std::string message, uint32_t color);
	void clear_console();
};

