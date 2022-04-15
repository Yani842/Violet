#include <iostream>

#include "src/engine.h"

class c1 : Violet::Component {};

class c2 : Violet::Component {};

class FireOB : public Violet::Object {
 public:
  FireOB() { AddComponent<c1>(); }
};

void listener() {}

int main(int argc, char** args) {
  Violet::Window win;
  win.Init("Test🙂", 1024, 768);
  auto mainLvl = win.CreateLevel("1", "2", "3");
  mainLvl->MarkListRenderable(1, "1");
  mainLvl->MarkListRenderable(2, "2");
  auto o = mainLvl->NewObject<FireOB>({0, 0, 24, 24}, "2", "3");
  mainLvl->UpdatableLists("3");
  auto o1 = mainLvl->NewObject<FireOB>({10, 10, 24, 24}, "1", "3");
  mainLvl->OnEvent(Violet::WpressEvent, listener);
  mainLvl->NewLists("4");
  mainLvl->AddObjectTo(o1, "4");

  std::cout << "start\n";
  for (auto obj : mainLvl->GetList("4")) {
    if (obj == nullptr) {
      std::cout << "null\n";
    } else {
      std::cout << "fire\n";
    }
  }
  win.Run();
  return 0;
}