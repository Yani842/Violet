#pragma once

#include <concepts>
#include <string>

// forward devlaration
namespace Violet {
class Object;
class Component;

namespace detail {

// opengl object id type
using id = uint32_t;
// for names of lists/windows/animations etc
using str = const char*;

// non empty string converable type
template <typename T>
concept NameConcept = requires(T a) {
  { a != "" } -> std::convertible_to<bool>;
  { std::is_convertible_v<T, std::string> } -> std::convertible_to<bool>;
};

// object derived type
template <typename T>
concept ObjectConcept = std::derived_from<T, Violet::Object>;

// component derived type
template <typename T>
concept ComponentConcept = std::derived_from<T, Violet::Component>;

}  // namespace detail
}  // namespace Violet