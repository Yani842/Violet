#include <iostream>

#include "Violet/Engine.h"

int main(int argc, char** args) {
  std::cout << "Hello, World!\n";
  Violet::Window win;
  win.Run("Hello", 1024, 768);
  return 0;
}