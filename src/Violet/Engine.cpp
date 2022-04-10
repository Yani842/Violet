#include "Engine.h"

#include <iostream>

void Violet::Window::Run(std::string title, int w, int h) {
  // initialize sdl and opengl
  SDL_Init(SDL_INIT_EVERYTHING);
  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
  SDL_GL_CreateContext(window);
  graphics = new Graphics();

  // timing
  Uint32 lastUpdate = SDL_GetTicks();

  // main loop
  while (running) {
    Uint64 start = SDL_GetTicks();

    // events
    SDL_Event* event;
    while (SDL_PollEvent(event)) {
      switch (event->type) {
        case SDL_QUIT:
          running = false;
          break;
      }
    }

    // update
    Uint32 current = SDL_GetTicks();
    float dt = (current - lastUpdate) / 1000.0f;

    // render
    graphics->Render();
    SDL_GL_SwapWindow(window);

    // timing
    lastUpdate = current;
    Uint64 end = SDL_GetTicks();
    auto delay = end - start;
    if (delay < 16.666f) {
      SDL_Delay(16.666f - delay);
    }
  }
}