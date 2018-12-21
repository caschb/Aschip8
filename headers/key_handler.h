#ifndef KEY_HANDLER_H
#define KEY_HANDLER_H
#include "SDL.h"

class KeyHandler{
	private:
		uint8_t * keys;
		void update_vector(SDL_Keycode &, uint8_t);
	public:
		KeyHandler(uint8_t *);
		~KeyHandler();
		bool handle_events();
		void clear();
};

#endif
