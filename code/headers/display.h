#ifndef DISPLAY_H
#define DISPLAY_H

#define SCREEN_ROWS 32
#define SCREEN_COLS 64
#define SCREEN_SIZE (SCREEN_ROWS * SCREEN_COLS)

#include <SDL_render.h>
#include <SDL_video.h>

class Display {
private:
  const uint8_t *display;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Rect rect;
  uint8_t pix_size;
  uint8_t scale;

public:
  Display(const uint8_t *);
  ~Display();
  void draw();
};

#endif
