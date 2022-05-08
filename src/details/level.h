#pragma once

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "globals.h"
#include "object.h"
#include "renderer.h"
#include "types.h"

namespace Violet {
namespace detail {
// Manages a vector that holds objects
class ObjectList {
 public:
  inline void Add(Violet::Object* obj) {
    objects.push_back(obj->shared_from_this());
  }
  inline void Remove(Violet::Object* obj) {
    auto find =
        std::find(objects.begin(), objects.end(), obj->shared_from_this());
    if (find != objects.end()) {
      objects.erase(find);
    }
  }
  inline std::vector<std::shared_ptr<Violet::Object>> Get() { return objects; }

 private:
  std::vector<std::shared_ptr<Violet::Object>> objects;
};
}  // namespace detail

namespace EV {
enum Events {
  pressW,
  pressA,
};
}

// Manages all the Objects using lists.
class Level {
 public:
  // Initializes renderer and creates lists.
  template <detail::NameConcept... listT>  // uses variadic templates.
  Level(detail::str texturePath, const listT&... lists)
      : renderer(texturePath) {
    detail::ActiveRenderer = &renderer;
    NewLists(lists...);
  }
  // Creates new object.
  template <detail::ObjectConcept objectT, detail::NameConcept... listT>
  inline Object* NewObject(detail::Rect rect, const listT&... lists) {
    std::shared_ptr<Object> obj(new objectT());
    AddObjectTo(obj.get(), lists...);
    obj->setPosition(rect.x, rect.y);
    obj->setSize(rect.w, rect.h);
    all.push_back(obj);
    return obj.get();
  }
  // Deletes an object from all the lists.
  // and from graphics
  template <detail::ObjectConcept objectT>
  objectT* DeleteObject(Object* object) {
    // stop rendering bcs it deletes from everytihnk including render lists
    object->StopRendering();
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
  template <detail::NameConcept... listT>
  void AddObjectTo(Object* object, const listT&... lists) {
    for (auto list : {lists...}) {
      map.at(list).Add(object);
    }
  }
  // Removes object from lists.
  template <detail::NameConcept... listT>
  void DeleteObjectFrom(Object* object, const listT&... lists) {
    for (auto list : {lists...}) {
      map.at(list).Remove(object);
    }
  }
  // Adds lists to the level.
  template <detail::NameConcept... listT>
  void NewLists(const listT&... lists) {
    for (auto list : {lists...}) {
      map.insert({list, detail::ObjectList()});
    }
  }
  // Deletes lists. The objects still exist.
  template <detail::NameConcept... listT>
  void DeleteLists(const listT&... lists) {
    for (auto list : {lists...}) {
      map.erase(list);
    }
  }
  // Makes the list update.
  template <detail::NameConcept... listT>
  void UpdateLists(const listT&... lists) {
    for (auto list : {lists...}) {
      updatable.push_back(list);
    }
  }
  // The lists will no lonFGET update.
  template <detail::NameConcept... listT>
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
  template <detail::NameConcept... listT>
  void StartRenderingLists(const listT&... lists) {
    for (auto list : {lists...}) {
      for (auto obj : map.at(list).Get()) {
        obj->StartRendering();
      }
    }
  }
  // starts rendering this specific object.
  inline void StartRenderingObject(Object* object) { object->StartRendering(); }
  // stops the lists's objects from rendering
  template <detail::NameConcept... listT>
  void StopRenderingLists(const listT&... lists) {
    for (auto list : {lists...}) {
      for (auto obj : map.at(list).Get()) {
        obj->StopRendering();
      }
    }
  }
  // stops rendering this specific object.
  inline void StopRenderingObject(Object* object) { object->StopRendering(); }
  // Creates new animation with these parameters.
  // creates animation and calls renderer's addanimation function with it
  inline void AddAnimation(const char* name, float rate,
                           std::initializer_list<detail::Rect> textures,
                           bool noAnimation = false, bool oneCycle = false) {
    renderer.AddAnimation(name, {textures, rate, noAnimation, oneCycle});
  }
  // event listener, when event happends listner will be called.
  void OnEvent(EV::Events event, std::function<void()> listener) {}
  // Calls Update to the updatable lists's objects.
  void Update(float dt) {
    for (auto list : updatable) {
      for (auto obj : map.at(list).Get()) {
        obj->UpdateAll(dt);
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
  std::unordered_map<std::string, detail::ObjectList> map;
  // all the draw stuff
  detail::Renderer renderer;
  // lists that will call update on their objects
  std::vector<std::string> updatable;
};

}  // namespace Violet