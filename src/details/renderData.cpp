#include "renderData.h"

void Violet::detail::RenderData::Update(float dt) {
  // dont do anything is its single texture or stopped
  if (animation->noAnimation || stopAnimation) {
    return;
  }
  // add the time passed from the prev update
  TimePassed += dt;

  currentTexture = static_cast<int>(std::floor(TimePassed / animation->rate));
  if (animation->oneCycle && currentTexture > animation->Textures.size()) {
    stopAnimation = true;
  } else {
    currentTexture %= animation->Textures.size();
    VBO.SetTexture(
        animation->Textures.at(currentTexture));  // optimise - not every time
  }
  if (TimePassed > animation->rate * animation->Textures.size()) {
    TimePassed = 0;
  }

  // if (TimePassed > animation->rate) {
  //   TimePassed = 0;
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