#pragma once

#include <memory>
#include <string>
#include <vector>

#include "globals.h"
#include "renderData.h"
#include "renderer.h"
#include "structs.h"

namespace Violet {
// forward declaration
class Level;
// a base class for all the components
class Component {
 public:
  virtual void Update(float dt);
  virtual ~Component() = 0;
};
// a base class for all the objcts the user will create
class Object : public std::enable_shared_from_this<Object> {
 public:
  // function that will call components update and then users update
  // ! update also the RD
  inline friend void UpdateObject(Object* obj, float dt) {
    obj->Update(dt);
  }
  // must be called before any Violet's function regarding the object
  Object() : level(ActiveLevel), renderer(detail::ActiveRenderer) {}
  // creates new component and adds it to the list.
  // ! unless the object already have one
  template <typename cmpT>
  void AddComponent() {}
  // deletes the component
  template <detail::ComponentConcept cmpT>
  void DeleteComponent() {}
  // set animation
  inline void SetAnimation(std::string animation, bool keepState = false) {
    RD.SetAnimation(renderer->GetAnimation(animation), keepState);
  }
  // called when pressed on it
  void GotPressed(int x, int y);
  // called when click releases on it
  void GotReleased(int x, int y);
  // stops animation
  void StopAnimation() {}
  // plays animation
  void ContinueAnimation() {}
  // generates and binds the needed information for rendering
  inline void StartRendering() { RD.VBO.Generate(); }
  // deletes and unbinds the needed information for rendering
  inline void StopRendering() { RD.VBO.Delete(); }
  // sets the coords of the top left corner
  inline void setPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
    RD.VBO.SetRect(rect);
  }
  // sets size
  inline void setSize(int w, int h) {
    rect.w = w;
    rect.h = h;
    RD.VBO.SetRect(rect);
  }
  // called if the object is in an updatable list.
  virtual void Update(float dt) {}
  // the level this object belongs to.
  Level* level;

 private:
  // object's level's renderer
  detail::Renderer* renderer;
  // holds all the rendering per objet information
  detail::RenderData RD;
  // position and size
  detail::Rect rect = {0, 0, 0, 0};
  // compomemt list
  std::vector<Component*> components;
};

}  // namespace Violet