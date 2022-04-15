#ifndef VIOLET_SRC_ENGINE_H
#define VIOLET_SRC_ENGINE_H

#include <SDL2/SDL.h>

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// future planning:
// - after finishing all the basic opengl stuff: multiple user choosed shaders
// per object option
// - make api for scroll with parallax effect.
// - make user proof functiond(i.e. in AddObjectTo check if levels already
// exist etc)
// - after opengl built-in components. make animation a component. movement,
// collision, etc.

// for tommorow:
// - clean the comments
// \ finish writing the components interface
// * write user interface to shaders
// \ add renderer gunctions to handle new/deleting vbo
// - start the writing of the functions definitions

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
concept ObjectConcept = std::derived_from<T, Object>;

template <typename T>
concept ComponentConcept = std::derived_from<Object, T>;

class ObjectList;

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
  // is in a layer
  bool renders = false;
  // vbo - opengl stuff
  id VBO;
  // the object's animation
  std::shared_ptr<Animation> animation;
  // is oneCycle animation ended one loop or the user decided to stop it
  bool stopAnimation = false;
  // how much time passed from the last texture change
  float pointInRate;
  // the index of the current rendering texture
  int currentTexture;
};

}  // namespace p

class Component {
 public:
  virtual void Update(float dt);
};

// a base class for all the objcts the user will create
class Object : public std::enable_shared_from_this<Object> {
 public:
  // initializes the object
  // template <typename... listT> , const listT&... lists
  friend void ObjectInit(std::shared_ptr<Object> obj, Level* level,
                         p::Rect rect) {}
  // function that will call components update and then users update
  friend void UpdateAll(Object* obj, float dt) {}

  // returns the RenderData
  friend p::RenderData* getRD(Object* obj) { return (&obj->RD); }

  // changes object's animation, resetState=false makes the new animation's
  // frame as the previous animation
  void SetAnimation(std::string animation, bool resetState = true);
  // creates new component and adds it to the list unless the onject alreasy
  // have one
  template <typename cmpT>
  void AddComponent() {}

  // deletes the component
  template <p::ComponentConcept cmpT>
  void DeleteComponent() {}
  // called when pressed on it
  void GotPressed(int x, int y) {}
  // called when click releases on it
  void GotReleased(int x, int y);
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
  // the level this object belongs to.
  Level* level;

 private:
  p::RenderData RD;
  // position and size
  p::Rect rect;
  // compomemt list
  std::vector<Component*> components;
};

namespace p {
// Manages a vector that holds objects
class ObjectList {
 public:
  inline void Add(Object* obj) { objects.push_back(obj->shared_from_this()); }

  inline void Remove(Object* obj) {
    auto find =
        std::find(objects.begin(), objects.end(), obj->shared_from_this());
    if (find != objects.end()) {
      objects.erase(find);
    }
  }

  inline std::vector<std::shared_ptr<Object>> Get() { return objects; }

 private:
  std::vector<std::shared_ptr<Object>> objects;
};

// responsible for drawing the objects(handling opengl)
class Renderer {
 public:
  // init shaders
  void Init() {}
  // Updates the RenderData of all the objects from the layers.
  void Update() {}
  // each frame draws objects
  // loop over each layer and draws it
  void Render() {}
  // adds list to the layer
  // bindind each vbo of all the list's objects to the layer's vao
  void Bind(int8_t layer, ObjectList list) {}
  // clears a layer
  // sets layer to null
  void Unbind(int8_t layer) {}
  // returns refernce to the animation
  // looks-up the kanimation and returns a reference to it
  Animation* GetAnimation(std::string animation) {
    return (map.at(animation).get());
  }
  // adds new animation to the map, called from level
  void addAnimation(std::string, Animation* animation) {}
  // initializes the vbo in the given renderState
  void initObject(Object* object) {}
  // deletes the given vbo in the renderState
  void deleteObject(Object* object) {}

 private:
  // 255 VAOs all set to 0
  id layers[sizeof(int8_t)] = {0};
  // a look-up map for animations - "name" : Animation
  std::unordered_map<std::string, std::shared_ptr<Animation>> map;
};

}  // namespace p

enum Events {
  WpressEvent,
  // etc...
};

// Manages all the Objects threw lists.
class Level {
 public:
  // Initializes renderer and creates lists.
  template <p::NameConcept... listT>  // uses variadic templates.
  Level(const listT&... lists) {
    NewLists(lists...);
  }

  /**
   * @brief Creates new object.
   *
   * @param rect object's position and size: {x, y, w, h}, where x and y are in
   * the top left corner.
   * @param lists names of the lists the object will belongs to.
   * @return pointer to the new object.
   */
  template <p::ObjectConcept objectT, p::NameConcept... listT>
  inline Object* NewObject(p::Rect rect, const listT&... lists) {
    std::shared_ptr<Object> obj(new objectT());
    ObjectInit(obj, this, rect);
    renderer.initObject(obj.get());
    AddObjectTo(obj.get(), lists...);
    return obj.get();
  }

  // Deletes an object from all the lists.
  // and from graphics
  template <p::ObjectConcept objectT>
  objectT* DeleteObject(Object* object) {
    for (auto list : map) {
      list.second.Remove(object);
    }
    renderer.deleteObject(object);
  }

  // Adds object to lists.
  template <p::NameConcept... listT>
  void AddObjectTo(Object* object, const listT&... lists) {
    for (auto list : {lists...}) {
      if (getRD(object)->renders) {
        std::cout << "Tried to add object to more than one renderable list."
                  << std::endl;
      } else {
        map.at(list).Add(object);
      }
    }
  }

  // Removes object from lists.
  template <p::NameConcept... listT>
  void DeleteObjectFrom(Object* object, const listT&... lists) {
    for (auto list : {lists...}) {
      map.at(list).Remove(object);
      if (getRD(object)->renders) {
        renderer.deleteObject(object);
        getRD(object)->renders = false;
      }
    }
  }

  // Adds lists to the level.
  template <p::NameConcept... listT>
  void NewLists(const listT&... lists) {
    for (auto list : {lists...}) {
      map.insert({list, p::ObjectList()});
    }
  }

  // Removes lists from the level.
  template <p::NameConcept... listT>
  void DeleteLists(const listT&... lists) {
    for (auto list : {lists...}) {
      map.erase(list);
    }
  }

  // Returns list's vector of shared ptr.
  template <p::NameConcept listT>
  inline std::vector<std::shared_ptr<Object>> GetList(listT list) {
    return map.at(list).Get();
  }

  // Makes the list updatable - the Update function of this list's objects
  // will be called every frame.
  template <p::NameConcept... listT>
  void UpdatableLists(const listT&... lists) {
    for (auto list : {lists...}) {
      updatable.push_back(list);
    }
  }

  // The lists will no longer update - the Update function of this list's
  // objects will no longer be called.
  template <p::NameConcept... listT>
  void UnmarkListsUpdatable(const listT&... lists) {
    for (auto list : {lists...}) {
      auto find = std::find(updatable.begin(), updatable.end(), list);
      if (find != updatable.end()) {
        updatable[find] = "";
      }
    }
  }

  // This list's object will be rendered at the layer. if there was a list at
  // this layer he will be overriten.
  template <p::NameConcept listT>
  void MarkListRenderable(uint8_t layer, const listT list) {
    // checks if the list already renders
    if (std::end(renderable) !=
        std::find(std::begin(renderable), std::end(renderable), list)) {
      std::cout
          << "you tried to make list to render even tho it already renders\n";
      return;
    }
    // checks if one of the list's object alredy renders in another listm
    for (auto obj : map.at(list).Get()) {
      if (getRD(obj.get())->renders) {
        std::cout << "you tried to make a list to render but one of its "
                     "objects already renders by another list!\n";
        return;
      }
    }
    // makes all list's object to be marked as renders to not be rendered by
    // another list on the future.
    for (auto obj : map.at(list).Get()) {
      getRD(obj.get())->renders = true;
    }
    // the renderer will do all the inside stuff
    renderer.Bind(layer, map.at(list));
    // to make sure it wont be called to render again
    renderable[layer] = list;
  }

  // Clears the layer - removes the list at this layer if existed. (expensive)
  void ClearLayer(int8_t layer) {
    renderer.Unbind(layer);
    for (auto obj : map.at(renderable[layer]).Get()) {
      getRD(obj.get())->renders = false;
    }
    renderable[layer] = nullptr;
  }

  // Creates new animation with these parameters.
  // creates animation and calls renderer's addanimation function with it
  void AddAnimation(std::string name, float rate,
                    std::initializer_list<p::Rect> textures,
                    bool noAnimation = false, bool oneCycle = false) {
    p::Animation animation{textures, rate, noAnimation, oneCycle};
    renderer.addAnimation(name, &animation);
  }

  // event listener, when event happends listner will be called.
  void OnEvent(Events event, std::function<void()> listener) {}

  // Calls Update to the updatable lists's objects.
  void Update(float dt) {
    for (auto list : updatable) {
      for (auto obj : map.at(list).Get()) {
        UpdateAll(obj.get(), dt);
      }
    }
  }

  // Renders the objects(by their animation) in the lists from the layers
  // (higher layers will be drawn over the lower layers).
  void Render() { renderer.Render(); }

 private:
  // list that contains all the objects (atleast one shared_ptr has to contain
  // an object at all times)
  std::vector<std::shared_ptr<Object>> all;
  // a look-up map for lists
  std::unordered_map<std::string, p::ObjectList> map;
  // all the draw stuff
  p::Renderer renderer;
  // lists that will call update on their objects
  std::vector<std::string> updatable;
  // all renderable lists by layer order to update the renderer's layer when
  // abject added/removed from renderable list
  std::string renderable[255];
};

// Manages the mainloop, timing and the window.
class Window {
 public:
  // Initializes Dependencies and creates window.
  void Init(std::string title, int w, int h) {
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(window);
  }

  // This level will manage all the objects.
  void SetLevel(Level* level) {
    delete p::ActiveLevel;
    p::ActiveLevel = level;
  }

  // Creates a level from template and sets this level.
  // Returns pointer to level.
  template <p::NameConcept... listT>  // uses variadic templates.
  Level* CreateLevel(const listT&... lists) {
    p::ActiveLevel = new Level(lists...);
    return &(*p::ActiveLevel);
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

#endif  // VIOLET_SRC_ENGINE_Hrenderable