#include "Engine.h"

#include <iostream>

#include <SDL2/SDL.h>

void Violet::Window::Init()
{
  SDL_Init(SDL_INIT_EVERYTHING);
  std::cout << "SDL init\n";
}