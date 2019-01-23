#include <fstream>


#include "console.h"
#include "renderer.h"
#include "macros.h"



console::console() {
	vertbarposition = 5; 
	history_position = command_history.begin();
	/* Initiate command map */
	command_map["quit"] = con_quit;
	command_map["clear"] = con_clear;
	command_map["init_server"] = con_init_server;
	command_map["init_client"] = con_init_client;
	command_map["connect"] = con_connect;
	command_map["send_testpacket"] = con_send_testpacket;
	command_map["receive_testpacket"] = con_receive_testpacket;

}

bool console::visible() { return vis; }
void console::setvisibility(bool visibility) { vis = visibility;  }

void console::toggle() {
	setvisibility(vis ^ 1);
	if (vis) 
		SDL_StartTextInput();
	else SDL_StopTextInput();
}

void console::draw_column(unsigned int timerTick) {
	static bool visible = false;

	if (timerTick % 30 == 0)
		visible ^= 1;

	if (visible) {
		for (int i = 0; i < CON_FONT_SIZE; ++i)
			draw_pixel(vertbarposition, VERT_BAR_Y_POS + i + 1, C_WHITE);
	}
}

void  console::inc_colpos(int val) {
	if (vertbarposition <= 210)
	vertbarposition += 7 * val; 
}
void  console::dec_colpos(int val) { 
		vertbarposition -= 7 * val;
	if (vertbarposition < 5)
		vertbarposition = 5;
}
void  console::reset_colpos() { vertbarposition = 5; }


std::string console::console_input_handler(SDL_Event &evnt) {
	
	if (evnt.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
		SDL_SetClipboardText(user_command.c_str());

	/* CTRL-V to paste */
	if (evnt.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
		user_command = SDL_GetClipboardText();
		reset_colpos();
		inc_colpos(user_command.size());
		return "";
	}

	/* Special cases */
	switch (evnt.key.keysym.sym) {
		case SDLK_BACKSLASH: return "";
		case SDLK_BACKSPACE: {
			if (!user_command.empty() || vertbarposition > 5) {
				user_command.erase(user_command.begin() + ((vertbarposition - 5) / 7) - 1);
				dec_colpos(1);
			}
			return "";
		}
		case SDLK_DELETE: {
			if (!user_command.empty() || vertbarposition < user_command.size())
				user_command.erase(user_command.begin() + ((vertbarposition - 5) / 7));
			return "";
		}
		case SDLK_LEFT: {
			if (vertbarposition > 5)
				dec_colpos(1);
			return "";
		}
		case SDLK_RIGHT: {
			if ((vertbarposition - 5) / 7 < user_command.size())
				inc_colpos(1);
			return "";
		}
		case SDLK_UP: {
			if (history_position != command_history.end()) {	
				user_command = *history_position;
				reset_colpos();
				inc_colpos(user_command.size());
				history_position++;
			}
			else {
				user_command.clear();
				reset_colpos();
			}
			return "";
		}
		case SDLK_DOWN: {
			if (history_position != command_history.begin()) {
				history_position--;
				user_command = *history_position;
				reset_colpos();
				inc_colpos(user_command.size());
			}
			else {
				user_command.clear();
				reset_colpos();
			}
			return "";
		}
		default:break;
	}

	/* Execute command */
	if (evnt.key.keysym.sym == SDLK_RETURN || evnt.key.keysym.sym == SDLK_KP_ENTER) {
		vertbarposition = 5;
		std::string cmd = user_command;
		user_command.erase(user_command.begin(), user_command.end());
		if (cmd != command_history.front())
			command_history.push_front(cmd);
		history_position = command_history.begin();
		return cmd;
	}

	/* Character input*/
	if (evnt.type == SDL_TEXTINPUT && (user_command.size() <= CONSOLE_INPUT_LIMT) && evnt.text.text[0] != '\\' ) {
		user_command += evnt.text.text[0];
		inc_colpos(1);
	}

	return "";
}

void console::write(std::string message, uint32_t color) {
	
	if (message.size() > 100)
		return;

	consolemessage msg;
	msg.message = message;
	msg.messagecolor = color;

	if (console_messages.size() > CONSOLE_DISPLAY_LIMIT)
		console_messages.pop();

	console_messages.push(msg);
	push_to_log(msg.message);
}

void console::write_to_log(std::string message) {
	consolemessage msg;
	msg.message = message;
	msg.messagecolor = C_BLACK;
	push_to_log(msg.message);
}

void console::clear_console() {
	console_messages = {};
}

void console::push_to_log(std::string msg) {
	console_log.push_back(msg);
}

void console::save_log_to_file() {
	std::ofstream file;

	file.open("gamefiles/console.log");
	
	unsigned int logsize = console_log.size();

	file << "LOG START" << std::endl << std::endl;

	for (unsigned int i = 0; i < logsize; ++i) 
		file << console_log.at(i) << std::endl;

	file << std::endl << "LOG END" << std::endl;

	file.close();
}

void console::write_welcome_message() {
	
	write("Asteroids version " GAME_VERSION " by " AUTHOR, C_NORMAL);
	write("Console version " CONSOLE_VERSION, C_NORMAL);
	write("Changelog: ", C_NORMAL);
	write("   *Can now use all unicode inputs ", C_SUCCESS);
	write("   *Can now copy from, and paste to, the console ", C_SUCCESS);
}

