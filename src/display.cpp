#include <cstdint>
#include "SDL.h"

#include "display.h"

Display::Display(const uint8_t (* screen_memory)[SCREEN_SIZE]){
	display = screen_memory;	
	scale = 10;
	pix_size = scale * scale;
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(SCREEN_COLS*scale, SCREEN_ROWS*scale, SDL_WINDOW_RESIZABLE, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
}

Display::~Display(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Display::draw(){
	int x_pos;
	int y_pos;
	for(int i = 0; i < SCREEN_ROWS; ++i){
		for(int j = 0; j < SCREEN_COLS; ++j){
			x_pos = i;
			y_pos = j * SCREEN_ROWS;
			rect.x = x_pos * pix_size;
			rect.y = y_pos * pix_size;
			rect.w = pix_size;
			rect.h = pix_size;
			if(display[x_pos + y_pos] == 0){
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			} else {
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			}
			SDL_RenderDrawRect(renderer, &rect);
		}
	}
	SDL_RenderPresent(renderer);
}
