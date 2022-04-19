#include <iostream>

#include "src/engine.h"

class c1 : Violet::Component {};

class c2 : Violet::Component {};

class FireOB : public Violet::Object {
 public:
  FireOB() {
    Init();
    AddComponent<c1>();
    SetAnimation("fire");
  }
};

void listener() {}

int main(int argc, char** args) {
  Violet::Window win;
  win.Init("Test🙂", 1024, 768);
  auto mainLvl = win.CreateLevel("data/textures.png", "1", "2", "3");
  mainLvl->AddAnimation("fire", 0, {{(60-30)/60.0f, (219-15)/219.0f,  20/60.0f, 27/219.0f}}, true); // 60x219
  mainLvl->UpdateLists("3");
  auto o1 = mainLvl->NewObject<FireOB>({-1, -1, 2, 2}, "1", "3");
  mainLvl->OnEvent(Violet::WpressEvent, listener);
  mainLvl->StartRenderingLists("2", "1");
  mainLvl->NewLists("4");
  mainLvl->AddObjectTo(o1, "4");
  
  win.Run();
  return 0;
}