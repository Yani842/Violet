#pragma once

// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
// clang-format on

#include <iostream>
#include <memory>

#include "structs.h"
#include "types.h"
#include "globals.h"
#include "renderer.h"

namespace Violet {
namespace detail {
// rendering data per object: vbo and animation state
class RenderData {
 public:
  class VBO {
   public:
    inline void Generate() {
      glGenBuffers(1, &vbo);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      // position
      // glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex),
      //                       (void*)offsetof(Vertex, pos));
      // glEnableVertexAttribArray(0);
      // // texture position in the texture atlas
      // glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex),
      //                       (void*)offsetof(Vertex, texPos));
      // glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      ActiveRenderer->AddVbo(vbo);
      renders = true;
    }
    inline void Update() {  // update only texpos / pos
      if (renders) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
      }
    }
    inline void Delete() {
      glDeleteBuffers(1, &vbo);
      renders = false;
      ActiveRenderer->DeleteVbo(vbo);
    }
    inline id Get() { return vbo; }
    inline void SetTexture(Rect rect) {
      // everything here is to transform from "pygame coords sys"
      // (top left is 0, 0 to screen size (i.e. 768, 1024)) to opengl coords sys
      rect.x = (60 - rect.x) / -60.0f;
      rect.y = (219 - rect.y) / -219.0f;
      rect.w /= 60.0f;
      rect.h /= 219.0f;
      vertices[0].texPos = {rect.x + rect.w, rect.y + rect.h, 1.0f};
      vertices[1].texPos = {rect.x, rect.y + rect.h, 1.0f};
      vertices[2].texPos = {rect.x, rect.y, 1.0f};
      vertices[3].texPos = {rect.x + rect.w, rect.y, 1.0f};
      Update();
    }
    inline void SetRect(Rect rect) {
      vertices[0].pos = {rect.x, rect.y, 1.0f};
      vertices[1].pos = {rect.x + rect.w, rect.y, 1.0f};
      vertices[2].pos = {rect.x + rect.w, rect.y + rect.h, 1.0f};
      vertices[3].pos = {rect.x, rect.y + rect.h, 1.0f};
      Update();
    }

   private:
    id vbo;
    // data for the vbo
    Vertex vertices[4];
    // to know if to call vbo.update
    bool renders = false;
  } VBO;
  // to free memory and stop rendering
  ~RenderData() { VBO.Delete(); }
  // updates the state accordinly to the current animation
  void Update(float dt);
  // sets aniamtion
  inline void SetAnimation(std::shared_ptr<Animation> animation_,
                           bool keepState = false) {
    animation = animation_;
    if (!keepState) {
      TimePassed = 0;
      currentTexture = 0;
    }
    stopAnimation = false;
    VBO.SetTexture(animation->Textures.at(0));
  }

 private:
  // for setting the texture
  std::shared_ptr<Animation> animation;
  // is oneCycle animation ended one loop or the user decided to stop it
  bool stopAnimation = false;
  // how much time passed from the last texture change
  float TimePassed = 0;
  // the index of the current rendering texture
  int currentTexture = 0;
};

}  // namespace detail
}  // namespace Violet