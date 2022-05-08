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

int main(int argc, char** args) {
  Violet::Window win;
  win.Init("Test🙂", 1024, 768);
  auto mainLvl = win.CreateLevel("data/textures.png", "update", "render", "ghosts");
  mainLvl->AddAnimation("ghost-idle", 0, {{30, 15, 20, 27}}, true);
  auto o1 = mainLvl->NewObject<GhostOB>({100, 100, 200, 200}, "update", "render", "ghosts");
  auto o2 = mainLvl->NewObject<GhostOB>({0, 0, 200, 200}, "update", "render", "ghosts");
  mainLvl->StartRenderingLists("render");
  mainLvl->UpdateLists("update");

  win.Run();
  return 0;
}