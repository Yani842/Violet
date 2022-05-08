#include <iostream>

#include "src/engine.h"

class c1 : Violet::Component {};

class c2 : Violet::Component {};

class GhostOB : public Violet::Object {
 public:
  GhostOB() : Object() {
    AddComponent<c1>();
    SetAnimation("ghost-idle");
  }
  void Update(float dt) override {
    // setPosition(positionX, 0);
    // positionX += 1;
  }
private:
  int positionX = 0;
};

// - camera class that is an object that controls the view matrix, posibly multiple but obly one active (threw level).
//   camera.rotate(...) rotateByMouse(sensi...) move(x y).
// - api for custom shaders
// - the new coordination system: bootom left is 0, 0 top right if SW, SH.
// - event handler(serach the sdl doc for that)
// - movement+collision components

int main(int argc, char** args) {
  Violet::Window win;
  win.Init("Test🙂", 1024, 768);
  auto mainLvl = win.CreateLevel("data/textures.png", "update", "render", "ghosts");
  mainLvl->AddAnimation("ghost-idle", 0.7, {{30, 15, 20, 27}, {0, 42, 20, 27}, {20, 42, 20, 27}}, false, true);
  auto o1 = mainLvl->NewObject<GhostOB>({0, 0, 200, 200}, "update", "render", "ghosts");
  auto o2 = mainLvl->NewObject<GhostOB>({200, 400, 200, 200}, "update", "render", "ghosts");
  mainLvl->StartRenderingLists("render");
  mainLvl->UpdateLists("update");

  win.Run();
  return 0;
}