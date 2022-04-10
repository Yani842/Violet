#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>

#include <string>
namespace Violet {
class Graphics {
 public:
  void Render();

 private:
  void Draw();
};
class Window {
 public:
  void Run(std::string title, int w, int h);

 private:
  bool running = true;
  SDL_Window* window = nullptr;
  Graphics* graphics = nullptr;
};
}  // namespace Violet