#include <SDL.h>
#include <key_handler.h>

KeyHandler::KeyHandler(uint8_t *keys) : keys(keys) {}

KeyHandler::~KeyHandler() {
  if (SDL_WasInit(SDL_INIT_VIDEO)) {
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
  }
}

void KeyHandler::update_vector(SDL_Keycode &key, uint8_t status) {
  if (key == SDLK_1) {
    keys[1] = status;
  } else if (key == SDLK_2) {
    keys[2] = status;
  } else if (key == SDLK_3) {
    keys[3] = status;
  } else if (key == SDLK_4) {
    keys[0xC] = status;
  } else if (key == SDLK_q) {
    keys[4] = status;
  } else if (key == SDLK_w) {
    keys[5] = status;
  } else if (key == SDLK_e) {
    keys[6] = status;
  } else if (key == SDLK_r) {
    keys[0xD] = status;
  } else if (key == SDLK_a) {
    keys[7] = status;
  } else if (key == SDLK_s) {
    keys[8] = status;
  } else if (key == SDLK_d) {
    keys[9] = status;
  } else if (key == SDLK_f) {
    keys[0xE] = status;
  } else if (key == SDLK_z) {
    keys[0xA] = status;
  } else if (key == SDLK_x) {
    keys[0] = status;
  } else if (key == SDLK_c) {
    keys[0xB] = status;
  } else if (key == SDLK_v) {
    keys[0xF] = status;
  }
}

void KeyHandler::clear() {
  for (int i = 0; i < 16; ++i) {
    keys[i] = 0;
  }
}

bool KeyHandler::handle_events() {
  SDL_Event e;
  bool quit = false;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      quit = true;
      break;
    case SDL_KEYDOWN:
      update_vector(e.key.keysym.sym, 1);
      break;
    case SDL_KEYUP:
      update_vector(e.key.keysym.sym, 0);
    }
  }
  return quit;
}
