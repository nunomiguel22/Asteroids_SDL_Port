

#include "console.h"
#include "renderer.h"
#include "macros.h"

console::console() {
	columnposition = 5; 
	
	consolemessage intro;
	intro.message = "Asteroids by Nuno Marques";
	intro.messagecolor = C_NORMAL;
	console_messages.push(intro);

	consolemessage version;
	version.message = "Console version 0.5";
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
		for (int i = 0; i < 20; ++i)
			draw_pixel(columnposition, 350 + i, C_WHITE);
	}
}

void  console::inc_colpos() {
	if (columnposition <= 305)
	columnposition += 10; 
}
void  console::dec_colpos() { 
		columnposition -= 10;
	if (columnposition < 5)
		columnposition = 5;
}
void  console::reset_colpos() { columnposition = 5; }

std::string console::console_input_handler(SDL_Event &evnt) {
	if (evnt.key.keysym.sym == SDLK_BACKSLASH)
		return "";

	if (evnt.key.keysym.sym == SDLK_BACKSPACE ) {
		if (!user_command.empty() || columnposition > 5) {
			user_command.erase( user_command.begin() + ((columnposition - 5) / 10) - 1);
			dec_colpos();
		}
		return "";
	}

	if (evnt.key.keysym.sym == SDLK_DELETE) {
		if (!user_command.empty() || columnposition < user_command.size()) 
			user_command.erase(user_command.begin() + ((columnposition - 5) / 10));
		return "";
	}


	if (evnt.key.keysym.sym == SDLK_LEFT) {
		if (columnposition > 5)
			dec_colpos();
		return "";
	}

	if (evnt.key.keysym.sym == SDLK_RIGHT) {
		if ((columnposition - 5) / 10 <  user_command.size())
			inc_colpos();
		return "";
	}

	if (evnt.key.keysym.sym == SDLK_SPACE) {
		if ((columnposition - 5) / 10 < 30) {
			inc_colpos();
			user_command += " ";
		}
		return "";
	}

	

	if (evnt.key.keysym.sym == SDLK_RETURN || evnt.key.keysym.sym == SDLK_KP_ENTER) {
		columnposition = 5;
		if (user_command == "quit") {
			std::string cmd = user_command;
			user_command.erase(user_command.begin(), user_command.end());
			write_message("Exiting...", C_NORMAL);
			return cmd;
		}
		else {
			user_command.erase(user_command.begin(), user_command.end());
			write_message("Command not found...", C_ERROR);
			return "";
		}
	}

	if (evnt.key.keysym.sym >= 97 && evnt.key.keysym.sym <= 122 && user_command.size() < 30) {
		user_command += evnt.key.keysym.sym;
		inc_colpos();
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

	if (console_messages.size() > 14)
		console_messages.pop();

	console_messages.push(msg);
}
