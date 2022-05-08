#pragma once

// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
// clang-format on

#include "Level.h"
#include "globals.h"
#include "types.h"

namespace Violet {
// Manages the mainloop, timing and the window.
class Window {
 public:
  // Initializes Dependencies and creates window.
  void Init(detail::str title, int w, int h);

  // Creates a level from template and sets this level.
  // Returns pointer to the level.
  template <detail::NameConcept... listT>  // uses variadic templates.
  inline Level* CreateLevel(const listT&... lists) {
    ActiveLevel = new Level(lists...);
    return &(*ActiveLevel);
  }

  // Starts the mainloop.
  void Run(bool printFps = false);

 private:
  // keep the game running
  bool running = true;
  // the window instance
  SDL_Window* window = nullptr;
};
}  // namespace Violet