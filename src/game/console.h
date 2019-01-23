#pragma once
#include <SDL.h>
#include <string>
#include <queue>
#include <map>
#include <list>
#include <vector>

typedef enum {	con_not_defined, con_init_server, con_init_client, con_connect, con_quit, con_clear, con_send_testpacket,
con_receive_testpacket} console_commands;

typedef struct {
	std::string message;
	uint32_t messagecolor;
}consolemessage;

class console {
	bool vis;
	unsigned int vertbarposition;								/* Vertical bar position */
	std::string user_command;									/* Current user command displayed on text input */
	std::queue <consolemessage> console_messages;				/* Current console messages displayed on console*/
	std::map <std::string, console_commands> command_map;		/* Map with all commands for easy comparison */
	std::list<std::string>::iterator history_position;			/* Current position on the command_history vector */
	std::list <std::string> command_history;					/* User command history */
	std::vector <std::string> console_log;						/* Log of all system messages */

public:
	console();
	~console(){};
	
	bool visible();
	std::string get_command() { return user_command; }
	int getvertbarposition() { return vertbarposition; }
	std::queue<consolemessage> getconsole_messages() { return console_messages; }
	std::map <std::string, console_commands>* get_command_map() { return &command_map; }

	void toggle();
	void setvisibility(bool visibility);
	void draw_column(unsigned int timerTick);
	void inc_colpos(int val);
	void dec_colpos(int val);
	void reset_colpos();

	std::string console_input_handler(SDL_Event &evnt);
	void write(std::string message, uint32_t color);
	void write_to_log(std::string message);
	void write_welcome_message();
	void clear_console();

	void push_to_log(std::string msg);
	void save_log_to_file();
};

