#include <cstdint>
#include "SDL.h"

#include "display.h"

Display::Display(const uint8_t * screen_memory){
	display = screen_memory;	
	scale = 5;
	
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(SCREEN_COLS*scale, SCREEN_ROWS*scale, SDL_WINDOW_RESIZABLE, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
}

Display::~Display(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	if(SDL_WasInit(SDL_INIT_VIDEO)){
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}
}

void Display::draw(){

	int x_pos;
	int y_pos;
	for(int i = 0; i < SCREEN_ROWS; ++i){
		for(int j = 0; j < SCREEN_COLS; ++j){
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			x_pos = j;
			y_pos = i * SCREEN_COLS;
			rect.x = x_pos * scale;
			rect.y = i * scale;
			rect.w = scale;
			rect.h = scale;
			if(display[x_pos + y_pos] == 1){
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			} 
			SDL_RenderFillRect(renderer, &rect);
		}
	}
	SDL_RenderPresent(renderer);
}
