#pragma once

// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
// clang-format on

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "shader.h"
#include "structs.h"
#include "types.h"

namespace Violet {
namespace detail {
// responsible for handling every opengl aspect
class Renderer {
 public:
  // init opengl objects
  Renderer(std::string textureAtlas);
  // draws objects
  inline void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    // glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }
  // returns shared ptr to the animation
  inline std::shared_ptr<Animation> GetAnimation(std::string animation) {
    std::unordered_map<std::string, std::shared_ptr<Animation>>::iterator it =
        map.find(animation);
    if (it == map.end()) {
      std::cout << "animation not found!\n";
      return map.at("invalid");  // implement!
    } else {
      return map.at(animation);
    }
  }
  // adds new animation to the map, called from level
  inline void AddAnimation(const char* name, Animation animation) {
    map[std::string(name)] = std::make_shared<Animation>(animation);
  }

 private:
  id VAO;
  id EBO;
  id texture;
  Shader shader;
  // map: (name: animation) the shared is bcs every RD has a pointer to it and
  // raw ptr wont work bcs map reallocates it's object
  std::unordered_map<std::string, std::shared_ptr<Animation>> map;
};
}  // namespace detail
}  // namespace Violet