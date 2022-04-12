#include "src/Engine.h"

class FireOB : public Violet::Object {};

int main(int argc, char** args) {

  Violet::Window win;
  win.Init("Hello", 1024, 768);
  Violet::Level startLV("1", "2", "3");
  startLV.MarkListRenderable(10, "2");
  win.SetLevel(&startLV);
  FireOB f;
  // f.Init(0, 0, 24, 24, "2", "1");
  startLV.AddObject(&f, "3");

  return 0;
}