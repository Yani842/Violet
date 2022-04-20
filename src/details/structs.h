#pragma once

#include <vector>

namespace Violet {
namespace detail {

// opengl object id type
using id = uint32_t;
// for names of lists/windows/animations etc
using str = const char*;

// simple rect struct
struct Rect {
  float x = 0;
  float y = 0;
  float w = 0;
  float h = 0;
};

// contains only information about an animation
struct Animation {
  // vector of the rects of the textures from the texture atlas
  const std::vector<Rect> Textures;
  // how much time passes between texture change (in seconds)
  const float rate = 0;
  // only one texture that doesn't change
  const bool noAnimation = false;
  // it loops over the texture once and disappears
  const bool oneCycle = false;
};

// all the data sent to the gpu per vertex
struct Vertex {
  struct Pos {
    float x = 0;
    float y = 0;
    float z = 0;
  } pos;
  struct texPos {
    float x = 0;
    float y = 0;
  } texPos;
};
}  // namespace detail
}  // namespace Violet