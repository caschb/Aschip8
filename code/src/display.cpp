#include <SDL.h>
#include <cstdint>
#include <display.h>

Display::Display(const uint8_t *screen_memory) {
  display = screen_memory;

  SDL_InitSubSystem(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(screen_cols * scale, screen_rows * scale,
                              SDL_WINDOW_RESIZABLE, &window, &renderer);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
}

Display::~Display() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  if (SDL_WasInit(SDL_INIT_VIDEO)) {
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
  }
}

void Display::draw() {

  int x_pos;
  int y_pos;
  for (int i = 0; i < screen_rows; ++i) {
    for (int j = 0; j < screen_cols; ++j) {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      x_pos = j;
      y_pos = i * screen_cols;
      rect.x = x_pos * scale;
      rect.y = i * scale;
      rect.w = scale;
      rect.h = scale;
      if (display[x_pos + y_pos] == 1) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      }
      SDL_RenderFillRect(renderer, &rect);
    }
  }
  SDL_RenderPresent(renderer);
}
