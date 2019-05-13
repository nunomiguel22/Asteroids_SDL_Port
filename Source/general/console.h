#pragma once

#include <SDL2/SDL_ttf.h>
#include <string>
#include <queue>
#include <map>
#include <list>
#include <vector>

typedef enum {	con_not_defined, con_init_server, con_init_client, con_init_client_localhost, con_host_game, con_connect, 
con_disconnect, con_quit, con_clear, con_send_testpacket, con_receive_testpacket, con_set_name, con_help} console_commands;

typedef struct {
	std::string message;
	uint32_t messagecolor;
}consolemessage;

class console {
private:

	/* Data */

	bool visible;
	unsigned int vertbar_pos;								/* Vertical bar position */
	std::string user_input;									/* Current user command displayed on text input */
	std::queue <consolemessage> console_msgs;				/* Current console messages displayed on console*/
	std::map <std::string, console_commands> cmd_map;		/* Map with all commands for easy comparison */
	std::list<std::string>::iterator history_position;			/* Current position on the command_history vector */
	std::list <std::string> command_history;					/* User command history */
	std::vector <std::string> console_log;						/* Log of all system messages */

	/* Functions */

	void vertbar_forward(int number_of_jumps);
	void vertbar_backward(int number_of_jumps);
	void vertbar_reset();

public:

	console();
	~console();
	
	bool is_open() const;
	std::string input() const;
	int vertbar_position() const;
	std::queue<consolemessage> get_console_messages() const;
	console_commands get_command(std::string command);
	void set_command(std::string command);
	
	void toggle();
	void open();
	void close();
	void clear();
	void write(std::string message, uint32_t color);
	void welcome_message();

	void log_push(std::string messsage);
	void log_save();

	std::string input_handler(SDL_Event &evnt);
	void vertbar_render(unsigned int timerTick);
};

