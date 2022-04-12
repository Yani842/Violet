#ifndef VIOLET_SRC_ENGINE_H
#define VIOLET_SRC_ENGINE_H

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

// future planning:
// - user can choose shaders
// - move objects via matrix (scroll and stuff)

namespace Violet {
namespace {  // private
// contains only information about an animation
struct Animation {
  // vector of the rects of the textures from the texture atlas
  std::vector<Rect> Textures;
  // how much time passes between texture change (in seconds)
  float rate;
  // only one texture that doesn't change
  bool noAnimation = false;
  // it loops over the texture once and disappears
  bool oneCycle = false;
};
}  // namespace
// public

using id = uint32_t;

// responsible for drawing the objects(handling opengl)
class Renderer {
 public:
  // init shaders
  void Inie();
  // each frame draws objects
  // loop over each layer and draws it
  void Render();
  // adds list to the layer
  // bindind each vbo of all the list's objects to the layer's vao
  void Bind(int8_t layer, ObjectList list);
  // clears a layer
  // sets layer to null
  void Unbind(int8_t layer);
  // returns refernce to the animation
  // looks-up the animation and returns a reference to it
  &Animation GetAnimation(std::string);
  // adds new animation to the map, called from level
  void addAnimation(std::string, Animation animation);

 private:
  // 255 VAOs all set to 0
  id layers[int8_t] = {0};
  // a look-up map for animations - "name" : Animation
  std::unordered_map<std::string, Animation> animations;
};

// self-explanatory
struct Rect {
  int x;
  int y;
  int w;
  int h;
}

struct Vertex {
  // pos
  // size
  // texpos
  // texsize
}

// each object has that - responsible for all the per object rnder information
class RenderData {
 public:
  RenderData() {
    // gen bind vertexattribpointer unbind
  }
  id VBO;
  Animation& animation;
  // is oneCycle animation ended one loop or the user decided to stop it
  bool stopAnimation = false;
  // how much time passed from the last texture change
  float pointInRate;
  // the index of the current rendering texture
  int currentTexture;
}

// a base class for all the objcts the user will create
class Object : private std::enable_shared_from_this<Object> {
 public:
  // initializes the object
  void Init(int x, int y, int w, int h, std::string lists[]);
  // changes object's animation, resetState=false makes the new animation's
  // frame as the previous animation
  void SetAnimation(std::string animation, resetState = true);
  // self-explanatory
  void StopAnimation();
  void ContinueAnimation();
  void setPosition(int x, int y);
  void setSize(int w, int h);
  // return shared_ptr og this object
  std::shared_ptr<Object> GetShared();
  // called if the object is in an updatable list - should be overriden by user
  virtual void Update(float dt);

 private:
  RenderData RD;
}

class ObjectList {
 public:
  void Add(shared_ptr<Object> obj);
  void Remove(shared_ptr<Object> obj);
  std::vector<std::shared_ptr<Object>> inline Get() { return objects; }

 private:
  std::vector<std::shared_ptr<Object>> objects;
};

// manages all the lists
class Level {
 public:
  // adds object to a list/s
  void AddObject(Object* obj, std::string list);
  void AddObject(Object* obj, std::string lists[]);
  // removes object from a list/s
  void RemoveObject(Object* obj, std::string list);
  void RemoveObject(Object* obj, std::string lists[]);
  // adds list/s to the map
  void Addlist(std::string name) void Addlist(std::string names[]);
  // removes lists from the map
  void Removelist(std::string name);
  void Removelist(std::string names[]);
  // this list's object's Update func will be called in Update
  void markListUpdatable(std::string name);
  void unmarkListUpdatable(std::string name);
  // this list will be rendered at layer until the layer changes/clears
  void markListRenderable(std::string name, int8_t layer);
  // sets layer to null (calls function)
  void clearLayer(int8_t layer);
  // creates animation and calls renderer's addanimation function with it
  void addAnimation(std::string name, float rate, std::vector<Rect> textures,
                    bool noAnimation = false, bool oneCycle = false);

  // updates all the objects from updatable lists
  void Update(float dt);
  // calls Renderer to render
  void Render();
  // delets all
  void Destroy();

 private:
  // a look-up map for lists
  std::unordered_map<std::string, ObjectList> ObjectList;
  // all the draw stuff
  Renderer renderer();
  // lists that eill call update on their objects
  std::vector<&ObjectList> updatable;
}

// manages current level, timing and window creation
class Window {
 public:
  // initializes sdl and opengl and creates window
  static void Init(std::string title, int w, int h);
  // deletes the precious level (is existed) and initializes the new level
  static void SetLevel(&Level);
  // main loop - calls level anf timing
  static Run();

 private:
  // self-explanatory
  static Level level;
  static bool running = true;
  static SDL_Window* window = nullptr;
};

}  // namespace Violet

#endif  // VIOLET_SRC_VIOLET_ENGINE_H