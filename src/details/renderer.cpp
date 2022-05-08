#include "renderer.h"

// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// clang-format on

inline void Violet::detail::Texture::Load(str path) {
  // glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  int width, height, nrChannels;
  // stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture\n";
  }
  stbi_image_free(data);
}

Violet::detail::Renderer::Renderer(str texturePath) {
  shader.init("data/shader.vs", "data/shader.fs");
  shader.use();

  // // init VAOs
  // glGenVertexArrays(1, &VAO);
  // init EBO - how to slice rects into triangles with the indices
  glGenBuffers(1, &EBO);
  // // bind vao to init it
  // glBindVertexArray(VAO);
  // every 3 is a triangle by the vertexes
  const unsigned int indices[6] = {0, 1, 3, 1, 2, 3};
  // bind indices to EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // // vertex setup
  // glGenVertexArrays(1, &VAO);

  // glBindVertexArray(VAO);
  // // positon
  // // Sets up the format, *without* a buffer object.
  // glVertexAttribFormat(0, 3, GL_FLOAT, GL_TRUE, offsetof(Vertex, pos));
  // // Sets up where the buffer object comes from
  // glVertexAttribBinding(0, 0);
  // // Enable VAO
  // glEnableVertexAttribArray(0);
  // // texture
  // // Sets up the format, *without* a buffer object.
  // glVertexAttribFormat(1, 2, GL_FLOAT, GL_TRUE, offsetof(Vertex, texPos));
  // // Sets up where the buffer object comes from
  // glVertexAttribBinding(1, 0);
  // // Enable VAO
  // glEnableVertexAttribArray(1);
  // //
  // glVertexBindingDivisor(1, 1);
  // // Done with VAO
  // glBindVertexArray(0);

  // to draw and then show, to show everything at the same time
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  // the color of the bg
  glClearColor(0.3f, 0.3f, 0.5f, 1.0f);
  // to allow transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  GLint textureUniform = glGetUniformLocation(shader.ID, "Texture");
  glUniform1i(textureUniform, 0);

  // transforms
  // glm::mat4 trans = glm::mat4(1.0f);
  // trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
  // trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f) =
      glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f), 2.0f / 2.0f, 0.1f, 100.0f);

  GLint modelLoc = glGetUniformLocation(shader.ID, "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  GLint viewLoc = glGetUniformLocation(shader.ID, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
  GLint projectionLoc = glGetUniformLocation(shader.ID, "projection");
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

  // create texture atlas
  texture.Load(texturePath);
}

void Violet::detail::Renderer::Render() {
  // u must have vector of vbos bcs vao can hold only 1 vbo at a time so the
  // only way to render multiple objects is to switch between vbos
  glClear(GL_COLOR_BUFFER_BIT);
  glBindVertexArray(VAO);
  for (auto vbo : VBOs) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // positon
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);
    // texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texPos));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }
  glBindVertexArray(0);
}