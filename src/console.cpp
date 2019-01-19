

#include "console.h"
#include "renderer.h"
#include "macros.h"


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
	if (columnposition <= 395)
	columnposition += 13; 
}
void  console::dec_colpos() { 
		columnposition -= 13;
	if (columnposition < 5)
		columnposition = 5;
}
void  console::reset_colpos() { columnposition = 5; }

int console::console_input_handler(SDL_Event &evnt) {
	if (evnt.key.keysym.sym == SDLK_BACKSLASH)
		return 0;

	if (evnt.key.keysym.sym == SDLK_BACKSPACE ) {
		if (!user_command.empty() || columnposition > 5) {
			user_command.erase( user_command.begin() + ((columnposition - 5) / 13) - 1);
			dec_colpos();
		}
		return 0;
	}

	if (evnt.key.keysym.sym == SDLK_DELETE) {
		if (!user_command.empty() || columnposition < user_command.size()) 
			user_command.erase(user_command.begin() + ((columnposition - 5) / 13));
		return 0;
	}


	if (evnt.key.keysym.sym == SDLK_LEFT) {
		if (columnposition > 5)
			dec_colpos();
		return 0;
	}

	if (evnt.key.keysym.sym == SDLK_RIGHT) {
		if ((columnposition - 5) / 13 <  user_command.size())
			inc_colpos();
		return 0;
	}

	if (evnt.key.keysym.sym == SDLK_RETURN || evnt.key.keysym.sym == SDLK_KP_ENTER) {
		columnposition = 5;
		if (user_command == "quit") {
			user_command.erase(user_command.begin(), user_command.end());
			write_message("Exiting...");
			return 1;
		}
		else {
			user_command.erase(user_command.begin(), user_command.end());
			write_message("Command not found...");
			return 0;
		}
	}

	if (evnt.key.keysym.sym >= 97 && evnt.key.keysym.sym <= 122 && user_command.size() < 30) {
		user_command += evnt.key.keysym.sym;
		inc_colpos();
	}
	return 0;
}

void console::write_message(std::string message) {
	if (message.size() > 100)
		return;


	if (console_messages.size() > 10)
		console_messages.pop();

	console_messages.push(message);
}
