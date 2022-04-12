#ifndef VIOLET_SRC_ENGINE_H
#define VIOLET_SRC_ENGINE_H

#include <SDL2/SDL.h>

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// future planning:
// - user can choose shaders
// - move objects via matrix (scroll and stuff)
// - user can make custem event handlers
// - add component system (physics, collidion, etc)
// \ hide all the sruff the user dont need.

// for tommorow:
// - clean the comments
// - finish writing the components interface
// - write user interface to shaders
// - add renderer gunctions to handle new/deleting vbo
// - start the writing of the functions descriptions

namespace Violet {
// forward declaration
class Object;
class Level;

namespace p {
// opengl object id type
using id = uint32_t;

extern Level* ActiveLevel;

template <typename T>
concept NameConcept = std::is_convertible_v<T, std::string>;

template <typename T>
concept ObjectConcept = std::derived_from<Object, T>;

template <typename T>
concept ComponentConcept = std::derived_from<Object, T>;

// A rect
struct Rect {
  int x;
  int y;
  int w;
  int h;
};

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

// all the data sent to the gpu per vertex
struct Vertex {
  // pos
  // size
  // texpos
  // texsize
};

// each object has that - responsible for all the per object rnder information
class RenderData {
 public:
  // gen bind vertexattribpointer unbind
  RenderData() {}

  id VBO;
  Animation* animation = nullptr;
  // is oneCycle animation ended one loop or the user decided to stop it
  bool stopAnimation = false;
  // how much time passed from the last texture change
  float pointInRate;
  // the index of the current rendering texture
  int currentTexture;
};

// manages a vector that holds objects
class ObjectList {
 public:
  void Add(Object* obj) { objects.push_back(obj->shared_from_this()); }

  inline void Remove(Object* obj) {
    objects.erase(
        std::find(objects.begin(), objects.end(), obj->shared_from_this()));
  }

  std::vector<std::shared_ptr<Object>> inline Get() { return objects; }

 private:
  std::vector<std::shared_ptr<Object>> objects;
};

// responsible for drawing the objects(handling opengl)
class Renderer {
 public:
  // init shaders
  void Init();
  // Updates the RenderData of all the objects from the layers.
  void Update();
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
  // looks-up the kanimation and returns a reference to it
  Animation& GetAnimation(std::string);
  // adds new animation to the map, called from level
  void addAnimation(std::string, Animation animation);

 private:
  // 255 VAOs all set to 0
  id layers[sizeof(int8_t)] = {0};
  // a look-up map for animations - "name" : Animation
  std::unordered_map<std::string, Animation> animations;
};

}  // namespace p

class Component {
 public:
  virtual void Update(float dt);
}

// a base class for all the objcts the user will create
class Object : private std::enable_shared_from_this<Object> {
 public:
  // initializes the object
  template <typename... listT>
  friend void ObjectInit(Object obj, Level* level, p::Rect rect,
                         const listT&... lists) {}
  // changes object's animation, resetState=false makes the new animation's
  // frame as the previous animation
  void SetAnimation(std::string animation, bool resetState = true);
  // creates new component and adds it to the list unless the onject alreasy
  // have one
  template <p::ComponentConcept cmpT>
  void AddComponent() {}
  // deletes the component
  template <p::ComponentConcept cmpT>
  void DeleteComponent() {}
  // self-explanatory
  void StopAnimation();
  void ContinueAnimation();
  void setPosition(int x, int y);
  void setSize(int w, int h);
  // return shared_ptr og this object
  std::shared_ptr<Object> GetShared();
  // called if the object is in an updatable list - should be overriden by
  // user
  virtual void Update(float dt) {}

 private:
  p::RenderData RD;
  // position and size
  p::Rect rect;
  // the level this object belongs to.
  Level* level;
  // compomemt list
  std::vector<Component*> components;
};

// Manages all the Objects threw lists.
class Level {
 public:
  // Initializes renderer and creates lists.
  template <p::NameConcept... listT>  // uses variadic templates.
  Level(const listT&... lists) {}

  // Creates new object from type and return pointer to it.
  template <p::ObjectConcept objectT, p::NameConcept... listT>
  objectT* NewObject(p::Rect rect, const listT&... lists) {
    Object ob;
    ObjectInit(ob, this, {0, 0, 24, 24}, "1", "4");
  }

  // Deletes an object from all the lists.
  // and from graphics
  template <p::ObjectConcept objectT, p::NameConcept... listT>
  objectT* DeleteObject(Object* object) {}

  // Adds object to lists.
  template <p::NameConcept... listT>
  void AddObject(Object* object, const listT&... lists) {}

  // Removes object from lists.
  template <p::NameConcept... listT>
  void RemoveObject(Object* object, const listT&... lists) {}

  // Adds lists to the level.
  template <p::NameConcept... listT>
  void NewLists(const listT&... lists) {}

  // Removes lists from the level.
  template <p::NameConcept... listT>
  void DeleteLists(const listT&... lists) {}

  // Makes the list updatable - the Update function of this list's objects
  // will be called every frame.
  template <p::NameConcept... listT>
  void UpdatableLists(const listT&... lists) {}

  // The lists will no longer update - the Update function of this list's
  // objects will no longer be called.
  template <p::NameConcept... listT>
  void UnmarkListsUpdatable(const listT&... lists) {}

  // This list's object will be rendered at the layer. if there was a list at
  // this layer he will be overriten.
  template <p::NameConcept... listT>
  void MarkListRenderable(uint8_t layer, const listT&... lists) {}

  // Clears the layer - removes the list at this layer if existed.
  void ClearLayer(int8_t layer) {}

  // Creates new animation with these parameters.
  // creates animation and calls renderer's addanimation function with it
  void AddAnimation(std::string name, float rate, std::vector<p::Rect> textures,
                    bool noAnimation = false, bool oneCycle = false) {}

  // Calls Update to the updatable lists's objects.
  void Update(float dt) {}

  // Renders the objects(by their animation) in the lists from the layers
  // (higher layers will be drawn over the lower layers).
  void Render() {}

  // Deletes all the lists and objects.
  void Destroy() {}

 private:
  // list that contains all the objects (atleast one shared_ptr has to contain
  // an object at all times)
  std::vector<std::shared_ptr<Object>> all;
  // a look-up map for lists
  std::unordered_map<std::string, p::ObjectList> ObjectList;
  // all the draw stuff
  p::Renderer renderer();
  // lists that eill call update on their objects
  std::vector<std::string> updatable;
};

// Manages the mainloop, timing and the window.
class Window {
 public:
  // Initializes and creates window.
  void Init(std::string title, int w, int h) {
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(window);
  }

  // This level will manage all the objects.
  void SetLevel(Level* level) {
    if (p::ActiveLevel) p::ActiveLevel->Destroy();
    p::ActiveLevel = level;
  }

  // Starts the mainloop.
  void Run() {
    Uint32 lastUpdate = SDL_GetTicks();
    // main loop
    while (running) {
      Uint64 start = SDL_GetTicks();

      // events
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        switch (event.type) {
          case SDL_QUIT:
            running = false;
            break;
        }
      }

      // update
      Uint32 current = SDL_GetTicks();
      float dt = (current - lastUpdate) / 1000.0f;
      p::ActiveLevel->Update(dt);

      // render
      p::ActiveLevel->Render();
      SDL_GL_SwapWindow(window);

      // timing
      lastUpdate = current;
      Uint64 end = SDL_GetTicks();
      auto delay = end - start;
      if (delay < 16.666f) {
        SDL_Delay(16.666f - delay);
      }
    }
  }

 private:
  // keep the game running
  bool running = true;
  // the window instance
  SDL_Window* window = nullptr;
};

}  // namespace Violet

#endif  // VIOLET_SRC_ENGINE_H