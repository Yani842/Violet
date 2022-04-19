#ifndef VIOLET_SRC_ENGINE_H
#define VIOLET_SRC_ENGINE_H

// #pragma pack(1)

// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
// clang-format on

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "shader.h"

// friend function makes the function access private members of given object
// with the type it was declared in and cant be accessed by derived classes
// and can by from the namespace

// future planning:
// - after finishing all the basic opengl stuff: multiple user choosed shaders
// per object option
// - make api for scroll with parallax effect.
// - do the event system.
// - convert as much code to algorithm functions and then inline.
// - make user proof functiond(i.e. make sure the object inits);
// - after opengl built-in components. make animation a component. movement,
// collision, etc.
// - create particle system.
// - objects have onclick and onkeypress stuff.
// - save files support (load from them, save to them etc)

// for tommorow:
// automaticly normalize the texture coords and object pos
// add custum shader api

namespace Violet {
// forward declaration
class Object;
class Level;
class Renderer;
// opengl object id type
using id = uint32_t;
// for names of lists/windows/animations etc
using str = const char*;
namespace p {
extern Level* ActiveLevel;
// concepts to prevent the user from typing in the wrong types or things
template <typename T>
concept NameConcept = requires(T a) {
  { a != "" } -> std::convertible_to<bool>;
  { std::is_convertible_v<T, std::string> } -> std::convertible_to<bool>;
};
template <typename T>
concept ObjectConcept = std::derived_from<T, Object>;
template <typename T>
concept ComponentConcept = std::derived_from<Object, T>;
class ObjectList;
struct Rect {
  float x = 0;
  float y = 0;
  float w = 0;
  float h = 0;
};
// contains only information about an animation
struct Animation {
  // vector of the rects of the textures from the texture atlas
  const std::vector<Rect> Textures;
  // how much time passes between texture change (in seconds)
  const float rate = 0;
  // only one texture that doesn't change
  const bool noAnimation = false;
  // it loops over the texture once and disappears
  const bool oneCycle = false;
};
// all the data sent to the gpu per vertex
struct Vertex {
  struct Pos {
    float x = 0;
    float y = 0;
    float z = 0;
  } pos;
  struct texPos {
    float x = 0;
    float y = 0;
  } texPos;
};
// each object has that - responsible for all the per object rnder information
class RenderData {
 public:
  class VBO {
   public:
    inline void Generate() {
      glGenBuffers(1, &vbo);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      // position
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(p::Vertex),
                            (void*)offsetof(p::Vertex, pos));
      glEnableVertexAttribArray(0);
      // texture position in the texture atlas
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(p::Vertex),
                            (void*)offsetof(p::Vertex, texPos));
      glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      renders = true;
    }
    inline void Update() {
      if (renders) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
      }
    }
    inline void Delete() {
      glDeleteBuffers(1, &vbo);
      renders = false;
    }
    inline id Get() { return vbo; }
    inline void SetTexture(Rect rect) {
      vertices[0].texPos = {rect.x, rect.y};
      vertices[1].texPos = {rect.x + rect.w, rect.y};
      vertices[2].texPos = {rect.x + rect.w, rect.y + rect.h};
      vertices[3].texPos = {rect.x, rect.y + rect.h};
      Update();
    }
    inline void SetRect(Rect rect) {
      vertices[0].pos = {rect.x, rect.y};
      vertices[1].pos = {rect.x + rect.w, rect.y};
      vertices[2].pos = {rect.x + rect.w, rect.y + rect.h};
      vertices[3].pos = {rect.x, rect.y + rect.h};
      Update();
    }

   private:
    id vbo;
    // data for the vbo
    p::Vertex vertices[4];
    // to know if to call vbo.update
    bool renders = false;
  } VBO;

  ~RenderData() { VBO.Delete(); }
  // updates the state accordinly to the current animation
  void Update(float dt);
  void SetAnimation(std::shared_ptr<Animation> animation_,
                    bool keepState = false) {
    animation = animation_;
    if (!keepState) {
      TimePassed = 0;
      currentTexture = 0;
    }
    stopAnimation = false;
    VBO.SetTexture(animation->Textures.at(0));
  }

 private:
  // for setting the texture
  std::shared_ptr<Animation> animation;
  // is oneCycle animation ended one loop or the user decided to stop it
  bool stopAnimation = false;
  // how much time passed from the last texture change
  float TimePassed = 0;
  // the index of the current rendering texture
  int currentTexture = 0;
};
}  // namespace p
class Component {
 public:
  virtual void Update(float dt);
  virtual ~Component() = 0;
};
// forward declaration
std::shared_ptr<p::Animation> GetAnimation(Level* level, std::string name);
// a base class for all the objcts the user will create
class Object : public std::enable_shared_from_this<Object> {
 public:
  // function that will call components update and then users update
  // ! update also the RD
  friend void UpdateObject(Object* obj, float dt) {}
  // must be called before any Violet's function regarding the object
  void Init() { level = p::ActiveLevel; }
  // returns the RenderData
  friend p::RenderData* GetRD(Object* obj) { return (&obj->RD); }
  // changes object's animation, state refers to the current texture.
  inline void SetAnimation(const char* animation, bool keepState = false) {
    RD.SetAnimation(GetAnimation(level, animation), keepState);
  }
  // creates new component and adds it to the list.
  // ! unless the object already have one
  template <typename cmpT>
  void AddComponent() {}
  // deletes the component
  template <p::ComponentConcept cmpT>
  void DeleteComponent() {}
  // called when pressed on it
  void GotPressed(int x, int y) {}
  // called when click releases on it
  void GotReleased(int x, int y);
  // stops animation
  void StopAnimation() {}
  // plays animation
  void ContinueAnimation();
  // sets the coords of the top left corner
  void setPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
    RD.VBO.SetRect(rect);
  }
  // sets size
  void setSize(int w, int h) {
    rect.w = w;
    rect.h = h;
    RD.VBO.SetRect(rect);
  }
  // called if the object is in an updatable list.
  virtual void Update(float dt) {}
  // the level this object belongs to.
  Level* level;

 private:
  p::RenderData RD;
  // position and size
  p::Rect rect = {0, 0, 0, 0};
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
  std::vector<std::shared_ptr<Object>> objects = {};
};
// responsible for drawing the objects(handling opengl)
class Renderer {
 public:
  // init opengl objects
  Renderer(std::string textureAtlas);
  // draws objects
  inline void Render() {
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, texture);
    // glClearColor(0.3f, 0.3f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }
  // returns shared ptr to the animation
  inline std::shared_ptr<Animation> GetAnimation(std::string animation) {
    std::unordered_map<std::string, std::shared_ptr<Animation>>::iterator it =
        map.find(animation);
    if (it == map.end()) {
      std::cout << "animation not found!\n";
      return map.at("invalid");
    } else {
      return map.at(animation);
    }
  }
  // adds new animation to the map, called from level
  inline void AddAnimation(const char* name, Animation animation) {
    map[std::string(name)] = std::make_shared<Animation>(animation);
  }

 private:
  // 255 VAOs all set to 0
  id VAO;
  id texture;
  Shader shader;
  // map: (name: animation) the shared is bcs every RD has a pointer to it and
  // raw ptr wont work bcs map reallocates it's object
  std::unordered_map<std::string, std::shared_ptr<Animation>> map;
  // the way the squares are sliced into trianlges
  id EBO;
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
  Level(std::string texturePath, const listT&... lists)
      : renderer(texturePath) {
    NewLists(lists...);
  }
  inline friend p::Renderer* GetRenderer(Level* level) {
    return &level->renderer;
  }
  inline friend std::shared_ptr<p::Animation> GetAnimation(Level* level,
                                                           std::string name) {
    return level->renderer.GetAnimation(name);
  }
  // Creates new object.
  template <p::ObjectConcept objectT, p::NameConcept... listT>
  inline Object* NewObject(p::Rect rect, const listT&... lists) {
    std::shared_ptr<Object> obj(new objectT());
    AddObjectTo(obj.get(), lists...);
    obj->setPosition(rect.x, rect.y);
    obj->setSize(rect.w, rect.h);
    all.push_back(obj);
    return obj.get();
  }
  // Deletes an object from all the lists.
  // and from graphics
  template <p::ObjectConcept objectT>
  objectT* DeleteObject(Object* object) {
    // stop rendering bcs it deletes from everytihnk including render lists
    GetRD(object)->VBO.Delete();
    // deletes from everylist
    for (auto list : map) {
      list.second.Remove(object);
    }
    // from all - destroys it
    auto i = std::find(all.begin(), all.end(), object);
    if (i != all.end()) {
      all.erase(i);
    }
  }
  // Adds object to lists.
  template <p::NameConcept... listT>
  void AddObjectTo(Object* object, const listT&... lists) {
    for (auto list : {lists...}) {
      map.at(list).Add(object);
    }
  }
  // Removes object from lists.
  template <p::NameConcept... listT>
  void DeleteObjectFrom(Object* object, const listT&... lists) {
    for (auto list : {lists...}) {
      map.at(list).Remove(object);
    }
  }
  // Adds lists to the level.
  template <p::NameConcept... listT>
  void NewLists(const listT&... lists) {
    for (auto list : {lists...}) {
      map.insert({list, p::ObjectList()});
    }
  }
  // Deletes lists. The objects still exist.
  template <p::NameConcept... listT>
  void DeleteLists(const listT&... lists) {
    for (auto list : {lists...}) {
      map.erase(list);
    }
  }
  // Makes the list update.
  template <p::NameConcept... listT>
  void UpdateLists(const listT&... lists) {
    for (auto list : {lists...}) {
      updatable.push_back(list);
    }
  }
  // The lists will no longer update.
  template <p::NameConcept... listT>
  void DontUpdateLists(const listT&... lists) {
    for (auto list : {lists...}) {
      auto i = std::find(updatable.begin(), updatable.end(), list);
      // to make sure the list exist in updatable
      if (i != updatable.end()) {
        updatable[i] = "";
      } else {
        std::cout << "list " << list << " do not update.\n";
      }
    }
  }
  // The lists's objects will be rendered
  template <p::NameConcept... listT>
  void StartRenderingLists(const listT&... lists) {
    for (auto list : {lists...}) {
      for (auto obj : map.at(list).Get()) {
        GetRD(obj.get())->VBO.Generate();
      }
    }
  }
  // starts rendering this specific object.
  void StartRenderingObject(Object* object) {
    GetRD(object)->VBO.Generate();
  }
  // stops the lists's objects from rendering
  template <p::NameConcept... listT>
  void StopRenderingLists(const listT&... lists) {
    for (auto list : {lists...}) {
      for (auto obj : map.at(list).Get()) {
        GetRD(obj.get())->VBO.Delete();
      }
    }
  }
  // stops rendering this specific object.
  void StopRenderingObject(Object* object) {
    GetRD(object)->VBO.Delete();
  }
  // Creates new animation with these parameters.
  // creates animation and calls renderer's addanimation function with it
  inline void AddAnimation(const char* name, float rate,
                           std::initializer_list<p::Rect> textures,
                           bool noAnimation = false, bool oneCycle = false) {
    renderer.AddAnimation(name, {textures, rate, noAnimation, oneCycle});
  }
  // event listener, when event happends listner will be called.
  void OnEvent(Events event, std::function<void()> listener) {}
  // Calls Update to the updatable lists's objects.
  void Update(float dt) {
    for (auto list : updatable) {
      for (auto obj : map.at(list).Get()) {
        UpdateObject(obj.get(), dt);
      }
    }
  }
  // Renders all the objects from render lists.
  inline void Render() { renderer.Render(); }

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
};
// Manages the mainloop, timing and the window.
class Window {
 public:
  // Initializes Dependencies and creates window.
  void Init(str title, int w, int h) {
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(window);
    glewInit();
  }

  // This level will manage all the objects.
  void SetLevel(Level* level) {
    delete p::ActiveLevel;
    p::ActiveLevel = level;
  }

  // Creates a level from template and sets this level.
  // Returns pointer to the level.
  template <p::NameConcept... listT>  // uses variadic templates.
  inline Level* CreateLevel(const listT&... lists) {
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