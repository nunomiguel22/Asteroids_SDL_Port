#include <fstream>
#include <SDL2/SDL.h>

#include "../general/console.h"
#include "../renderer/renderer.h"
#include "../general/macros.h"


console::console() {
	visible = false;
	vertbar_pos = 5; 
	history_position = command_history.begin();

	/* Initiate command map */
	cmd_map["quit"] = con_quit;
	cmd_map["clear"] = con_clear;
	cmd_map["init_server"] = con_init_server;
	cmd_map["init_client"] = con_init_client;
	cmd_map["init_client_localhost"] = con_init_client_localhost;
	cmd_map["host_game"] = con_host_game;
	cmd_map["connect"] = con_connect;
	cmd_map["disconnect"] = con_disconnect;
	cmd_map["send_testpacket"] = con_send_testpacket;
	cmd_map["receive_testpacket"] = con_receive_testpacket;
	cmd_map["set_name"] = con_set_name;
	cmd_map["help"] = con_help;

}
console::~console() {}

/* Return data */

bool console::is_open() const { return visible; }
std::string console::input() const { return user_input; }
int console::vertbar_position() const { return vertbar_pos; }
std::queue<consolemessage> console::get_console_messages() const { return console_msgs; }
console_commands console::get_command(std::string command) { return cmd_map[command]; }
void console::set_command(std::string command) { user_input = command; vertbar_reset(); vertbar_forward(user_input.size()); }

/* Console functions */

void console::toggle() {
	visible ^= 1;
	if (visible)
		SDL_StartTextInput();
	else SDL_StopTextInput();
}

void console::open() {
	visible = true;
	SDL_StartTextInput();
}

void console::close() {
	visible = false;
	SDL_StopTextInput();
}

void console::clear() {
	console_msgs = {};
}

void console::write(std::string message, uint32_t color) {

	if (message.size() > 100)
		return;

	consolemessage msg;
	msg.message = message;
	msg.messagecolor = color;

	if (console_msgs.size() > CONSOLE_DISPLAY_LIMIT)
		console_msgs.pop();

	console_msgs.push(msg);
	log_push(msg.message);
}

void console::welcome_message() {

	write("Asteroids version " GAME_VERSION " by " AUTHOR, C_NORMAL);
	write("Console version " CONSOLE_VERSION, C_NORMAL);
	write("Changelog: ", C_NORMAL);
	write("   *Fixed input between characters appearing at the end", C_SUCCESS);
	write("   *Fixed backspace crashing the game when the input was empty", C_SUCCESS);
	write("   *Can now use console while playing, the game will be paused in singleplayer mode", C_SUCCESS);
}


/* Console log Functions */


void console::log_push(std::string messsage) {
	console_log.push_back(messsage);
}

void console::log_save() {
	std::ofstream file;

	file.open("gamefiles/console.log");

	unsigned int logsize = console_log.size();

	file << "LOG START" << std::endl << std::endl;

	for (unsigned int i = 0; i < logsize; ++i)
		file << console_log.at(i) << std::endl;

	file << std::endl << "LOG END" << std::endl;

	file.close();
}


/* Vertbar functions */

void console::vertbar_render(unsigned int timerTick) {
	static bool visible = false;

	if (timerTick % 30 == 0)
		visible ^= 1;

	if (visible) {
		for (int i = 0; i < CON_FONT_SIZE; ++i)
			draw_pixel(vertbar_pos, VERT_BAR_Y_POS + i + 1, C_WHITE);
	}
}

void  console::vertbar_forward(int number_of_jumps) {
	if (vertbar_pos <= 210)
		vertbar_pos += 7 * number_of_jumps;
}

void  console::vertbar_backward(int number_of_jumps) {
	vertbar_pos -= 7 * number_of_jumps;
	if (vertbar_pos < 5)
		vertbar_pos = 5;
}

void  console::vertbar_reset() { vertbar_pos = 5; }

/* Misc functions*/

std::string console::input_handler(SDL_Event &evnt) {
	
	if (evnt.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
		SDL_SetClipboardText(user_input.c_str());

	/* CTRL-V to paste */
	if (evnt.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
		user_input = SDL_GetClipboardText();
		vertbar_reset();
		vertbar_forward(user_input.size());
		return "";
	}

	//TODO
	/* Special cases */
	switch (evnt.key.keysym.sym) {
		case SDLK_BACKSLASH: return "";
		case SDLK_BACKSPACE: {
			if (!user_input.empty() && vertbar_pos > 5) {
				user_input.erase(user_input.begin() + ((vertbar_pos - 5) / 7) - 1);
				vertbar_backward(1);
			}
			return "";
		}
		case SDLK_DELETE: {
			if (!user_input.empty() || vertbar_pos < user_input.size())
				user_input.erase(user_input.begin() + ((vertbar_pos - 5) / 7));
			return "";
		}
		case SDLK_LEFT: {
			if (vertbar_pos > 5)
				vertbar_backward(1);
			return "";
		}
		case SDLK_RIGHT: {
			if ((vertbar_pos - 5) / 7 < user_input.size())
				vertbar_forward(1);
			return "";
		}
		case SDLK_UP: {
			if (history_position != command_history.end()) {	
				user_input = *history_position;
				vertbar_reset();
				vertbar_forward(user_input.size());
				history_position++;
			}
			else {
				user_input.clear();
				vertbar_reset();
			}
			return "";
		}
		case SDLK_DOWN: {
			if (history_position != command_history.begin()) {
				history_position--;
				user_input = *history_position;
				vertbar_reset();
				vertbar_forward(user_input.size());
			}
			else {
				user_input.clear();
				vertbar_reset();
			}
			return "";
		}
		default:break;
	}

	/* Execute command */
	if (evnt.key.keysym.sym == SDLK_RETURN || evnt.key.keysym.sym == SDLK_KP_ENTER) {
		vertbar_pos = 5;
		std::string cmd = user_input;
		user_input.erase(user_input.begin(), user_input.end());
		if (cmd != command_history.front())
			command_history.push_front(cmd);
		history_position = command_history.begin();
		return cmd;
	}

	/* Character input*/
	if (evnt.type == SDL_TEXTINPUT && (user_input.size() <= CONSOLE_INPUT_LIMT) && evnt.text.text[0] != '\\' ) {
		user_input.insert(user_input.begin() + ((vertbar_pos - 5) / 7), evnt.text.text[0]);
		vertbar_forward(1);
	}

	return "";
}
