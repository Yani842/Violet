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
// loads and binds the texture in the given path
class Texture {
 public:
  void Load(str path);
  id texture;
};
// responsible for handling every opengl aspect
class Renderer {
 public:
  // init opengl objects
  Renderer(str textureAtlas);
  // draws objects
  void Render();

  // start rendering this object
  inline void AddVbo(id vbo) { VBOs.push_back(vbo); }
  // stop rendering this object
  inline void DeleteVbo(id vbo) {
    auto find = std::find(VBOs.begin(), VBOs.end(), vbo);
    if (find != VBOs.end()) {
      VBOs.erase(find);
    }
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
  // buffers
  id VAO;
  id EBO;
  std::vector<id> VBOs;
  // handlers for specific topics
  Texture texture;
  Shader shader;
  // map: (name: animation) the shared is bcs every RD has a pointer to it and
  // raw ptr wont work bcs map reallocates it's object
  std::unordered_map<std::string, std::shared_ptr<Animation>> map;
};
}  // namespace detail
}  // namespace Violet