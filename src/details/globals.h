#pragma once

namespace Violet {
class Level;
extern Level* ActiveLevel;
namespace detail {
class Renderer;
extern Renderer* ActiveRenderer;
}  // namespace detail
}  // namespace Violet