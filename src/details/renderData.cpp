#include "renderData.h"

void Violet::detail::RenderData::Update(float dt) {
  // dont do anything is its single texture or stopped
  if (animation->noAnimation || stopAnimation) {
    return;
  }
  // add the time passed from the prev update
  TimePassed += dt;

  currentTexture = static_cast<int>(std::floor(TimePassed / animation->rate));
  if (animation->oneCycle && currentTexture > animation->Textures.size() - 1) {
    stopAnimation = true;
  } else {
    currentTexture %= animation->Textures.size();
    VBO.SetTexture(
        animation->Textures.at(currentTexture));  // optimise - not every time
  }
  if (TimePassed > animation->rate * animation->Textures.size()) {
    TimePassed = 0;
  }
}