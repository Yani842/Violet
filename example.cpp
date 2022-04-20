#include <iostream>

#include "src/engine.h"

class c1 : Violet::Component {};

class c2 : Violet::Component {};

class FireOB : public Violet::Object {
 public:
  FireOB() : Object() {
    AddComponent<c1>();
    SetAnimation("fire");
  }
};

int main(int argc, char** args) {
  Violet::Window win;
  win.Init("Test🙂", 1024, 768);
  auto mainLvl = win.CreateLevel("data/textures.png", "1", "2", "3");
  mainLvl->AddAnimation("fire", 0, {{30, 15, 20, 27}}, true);
  mainLvl->UpdateLists("3");
  auto o1 = mainLvl->NewObject<FireOB>({-1, -1, 2, 2}, "1", "3");
  mainLvl->StartRenderingLists("2", "1");
  mainLvl->NewLists("4");
  mainLvl->AddObjectTo(o1, "4");

  win.Run();
  return 0;
}