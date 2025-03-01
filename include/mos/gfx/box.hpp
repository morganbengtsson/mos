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
  [[nodiscard]] auto position() const -> glm::vec3;
  [[nodiscard]] auto size() const -> glm::vec3;
  [[nodiscard]] auto min() const -> glm::vec3;
  [[nodiscard]] auto max() const -> glm::vec3;
  [[nodiscard]] auto inside(const glm::vec3 &point) const -> bool;
};
} // namespace mos::gfx
