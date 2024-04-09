#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>

constexpr uint32_t screen_rows = 32;
constexpr uint32_t screen_cols = 64;
constexpr uint32_t screen_size = screen_rows * screen_cols;
constexpr uint8_t scale = 5;

#include <SDL_render.h>
#include <SDL_video.h>

class Display {
private:
  const uint8_t *display;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Rect rect;

public:
  Display(const uint8_t *);
  ~Display();
  void draw();
};

#endif
