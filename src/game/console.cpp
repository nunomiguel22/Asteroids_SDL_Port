

#include "console.h"
#include "renderer.h"
#include "macros.h"

console::console() {
	vertbarposition = 5; 

	/* Initiate command map */
	command_map["quit"] = con_quit;
	command_map["clear"] = con_clear;
	command_map["init_server"] = con_init_server;
	command_map["init_client"] = con_init_client;
	command_map["connect"] = con_connect;
	command_map["send_testpacket"] = con_send_testpacket;
	command_map["receive_testpacket"] = con_receive_testpacket;


	consolemessage intro;
	intro.message = "Asteroids version 1.7 by Nuno Marques";
	intro.messagecolor = C_NORMAL;
	console_messages.push(intro);

	consolemessage version;
	version.message = "Console version 0.8";
	version.messagecolor = C_NORMAL;

	console_messages.push(version);
}



bool console::visible() { return vis; }
void console::setvisibility(bool visibility) { vis = visibility;  }

void console::draw_column(unsigned int timerTick) {
	static bool visible = false;

	if (timerTick % 30 == 0)
		visible ^= 1;

	if (visible) {
		for (int i = 0; i < CON_FONT_SIZE; ++i)
			draw_pixel(vertbarposition, VERT_BAR_Y_POS + i + 1, C_WHITE);
	}
}

void  console::inc_colpos() {
	if (vertbarposition <= 210)
	vertbarposition += 7; 
}
void  console::dec_colpos() { 
		vertbarposition -= 7;
	if (vertbarposition < 5)
		vertbarposition = 5;
}
void  console::reset_colpos() { vertbarposition = 5; }

std::string console::console_input_handler(SDL_Event &evnt) {
	
	/* Special cases */
	switch (evnt.key.keysym.sym) {
		case SDLK_BACKSLASH: return "";
		case SDLK_BACKSPACE: {
			if (!user_command.empty() || vertbarposition > 5) {
				user_command.erase(user_command.begin() + ((vertbarposition - 5) / 7) - 1);
				dec_colpos();
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
				dec_colpos();
			return "";
		}
		case SDLK_RIGHT: {
			if ((vertbarposition - 5) / 7 < user_command.size())
				inc_colpos();
			return "";
		}
		case SDLK_SPACE: {
			if ((vertbarposition - 5) / 7 < 30) {
				inc_colpos();
				user_command += " ";
			}
			return "";
		}
		case SDLK_PERIOD: {
			if (SDL_GetModState() & KMOD_LSHIFT) {
				user_command += ':';
				inc_colpos();
			}
			else {
				user_command += '.';
				inc_colpos();
			}
			return "";
		}
		case SDLK_MINUS:{
			if (SDL_GetModState() & KMOD_LSHIFT) {
				user_command += '_';
				inc_colpos();
			}
			else {
				user_command += '-';
				inc_colpos();
			}
			return "";
		}
		case SDLK_COMMA: {
			if (SDL_GetModState() & KMOD_LSHIFT) {
				user_command += ';';
				inc_colpos();
			}
			else {
				user_command += ',';
				inc_colpos();
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
		return cmd;
	}

	/* Character input */
	if ((evnt.key.keysym.sym >= 97 && evnt.key.keysym.sym <= 122 && user_command.size() < 30)) {
		if (SDL_GetModState() & KMOD_LSHIFT) {
			user_command += evnt.key.keysym.sym - 32;
			inc_colpos();
		}
		else {
			user_command += evnt.key.keysym.sym;
			inc_colpos();
		}
	}

	else if (evnt.key.keysym.sym >= 48 && evnt.key.keysym.sym <= 57 && user_command.size() < 30) {
		user_command += evnt.key.keysym.sym;
		inc_colpos();
	}

	return "";
}

void console::write_message(std::string message, uint32_t color) {
	
	if (message.size() > 100)
		return;

	consolemessage msg;
	msg.message = message;
	msg.messagecolor = color;

	if (console_messages.size() > 28)
		console_messages.pop();

	console_messages.push(msg);
}

void console::clear_console() {
	console_messages = {};
}
