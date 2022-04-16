#include "engine.h"

using namespace Violet::p;

RenderData::RenderData() { glGenBuffers(1, &VBO); }

void RenderData::Update(float dt) {
  // dont do anything is its single texture or stopped
  if (animation->noAnimation || stopAnimation) {
    return;
  }
  // add the time passed from the prev update
  pointInRate += dt;

  currentTexture = static_cast<int>(std::floor(pointInRate / animation->rate));
  if (animation->oneCycle && currentTexture > animation->Textures.size()) {
    stopAnimation = true;
  } else {
    currentTexture %= animation->Textures.size();
  }
  if (pointInRate > animation->rate * animation->Textures.size()) {
    pointInRate = 0;
  }

  // if (pointInRate > animation->rate) {
  //   pointInRate = 0;
  //   currentTexture++;
  //   // it was the last texture in order
  //   if (currentTexture >= animation->Textures.size()) {
  //     currentTexture = 0;
  //     // the animation do not repeat
  //     if (animation->oneCycle) {
  //       stopAnimation = true;
  //     }
  //   }
  // }
}

Renderer::Renderer(std::string textureAtlas) {
  // init VAOs
  glGenVertexArrays(255, layers);
  // init EBO - how to slice rects into triangles with the indices
  glGenBuffers(1, &EBO);
  // to draw and then show, to show everything at the same time
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  // the color of the bg
  glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
  // to allow transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC0_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::Bind(int8_t layer, ObjectList* list) {
  glBindVertexArray(layers[layer]);
  // bind indices to EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  // bind all vbos to layer(vao)
  for (auto obj : list->Get()) {
    glBindBuffer(GL_ARRAY_BUFFER, getRD(obj.get())->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(p::Vertex) * 4,
                 getRD(obj.get())->vertices, GL_STATIC_DRAW);
  }
  // decipher Vertex to opengl
  // position
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(p::Vertex), (void*)offsetof(p::Vertex, pos));
  glEnableVertexAttribArray(0);
  // size
  glVertexAttribPointer(1, 2, GL_INT, GL_FALSE, sizeof(p::Vertex), (void*)offsetof(p::Vertex, size));
  glEnableVertexAttribArray(1);
  // texture position in the texture atlas
  glVertexAttribPointer(2, 2, GL_INT, GL_FALSE, sizeof(p::Vertex), (void*)offsetof(p::Vertex, tex.pos));
  glEnableVertexAttribArray(2);
  // texture width and height in the texture atlas
  glVertexAttribPointer(3, 2, GL_INT, GL_FALSE, sizeof(p::Vertex), (void*)offsetof(p::Vertex, tex.size));
  glEnableVertexAttribArray(3);
  // unbind layer
  glBindVertexArray(0);
}

void Renderer::Unbind(int8_t layer) { glDeleteVertexArrays(1, &layers[layer]); }
void Renderer::AddAnimation(std::string, Animation animation) {}
void Renderer::InitObject(RenderData* RD) {}
void Renderer::DeleteObject(RenderData* RD) {}
void Renderer::Render() {}
