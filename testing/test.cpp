#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Object : public std::enable_shared_from_this<Object> {
 public:
  Object() {}
  ~Object() {}
  std::string name = "Object";
};

class Wall : public Object {
 public:
  Wall() { name = "Wall"; }
};

class Player : public Object {
 public:
  Player() { name = "Player"; }
};

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

class AllObjectList {
 public:
  void Add(std::shared_ptr<Object> obj) { objects.push_back(obj); }

  inline void Remove(Object* obj) {
    objects.erase(
        std::find(objects.begin(), objects.end(), obj->shared_from_this()));
  }

  std::vector<std::shared_ptr<Object>> inline Get() { return objects; }

 private:
  std::vector<std::shared_ptr<Object>> objects;
};

// consept to make sure T derived from Object
class Level {
 public:
  AllObjectList All;
  template <typename T>
  T* NewObj() {
    std::shared_ptr<T> obj(new T());
    All.Add(obj);
    return &(*obj);
  }
};

int main() {
  Level level;
  ObjectList l1;
  auto pl = level.NewObj<Player>();
  l1.Add(pl);
  auto pl2 = level.NewObj<Player>();
  l1.Add(pl2);
  auto wa = level.NewObj<Wall>();
  l1.Add(wa);

  l1.Remove(pl2);
  l1.Add(pl2);

  for (auto obj : l1.Get()) {
    if (obj == nullptr) {
      std::cout << "null\n";
    } else {
      std::cout << obj->name << "\n";
    }
  }
  return 0;
}