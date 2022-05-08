#include "window.h"

void Violet::Window::Init(detail::str title, int w, int h) {
  SDL_Init(SDL_INIT_EVERYTHING);
  window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
  SDL_GL_CreateContext(window);
  glewInit();
}

void Violet::Window::Run(bool printFps) {
  Uint32 lastUpdate = SDL_GetTicks();
  Uint64 start_;
  // main loop
  while (running) {
    if (printFps) {
      start_ = SDL_GetPerformanceCounter();
    }
    Uint64 start = SDL_GetTicks();

    // events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          running = false;
          break;
        default:
          break;
      }
    }

    // update
    Uint32 current = SDL_GetTicks();
    float dt = (current - lastUpdate) / 1000.0f;
    ActiveLevel->Update(dt);

    // render
    ActiveLevel->Render();
    SDL_GL_SwapWindow(window);

    // timing
    lastUpdate = current;
    Uint64 end = SDL_GetTicks();
    auto elapsed = end - start;
    if (elapsed < 16.666f && !printFps) {
      SDL_Delay(16.666f - elapsed);
    }
    if (printFps) {
      Uint64 end_ = SDL_GetPerformanceCounter();
      float elapsed_ = (end_ - start_) / static_cast<float>(SDL_GetPerformanceFrequency());
      std::cout << "Current FPS: " << 1.0f / elapsed_ << "\n";
    }
  }
}