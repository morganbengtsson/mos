#pragma once

#include <glm/glm.hpp>

namespace mos::gfx {

/** Visual bounding box. */
class Box final {
public:
  Box();
  Box(const glm::mat4 &transform, const glm::vec3 &extent);
  glm::mat4 transform;
  glm::vec3 extent;
  auto position() const -> glm::vec3;
  auto size() const -> glm::vec3;
  auto min() const -> glm::vec3;
  auto max() const -> glm::vec3;
  auto inside(const glm::vec3 &point) const -> bool;
};
} // namespace mos::gfx
