#pragma once

#include <glm/glm.hpp>

namespace mos {
namespace gfx {

/** Bounding box for rendering. */
class Box final {
public:
  Box();
  Box(const glm::mat4 &transform,
      const glm::vec3 &extent);
  glm::mat4 transform;
  glm::vec3 extent;
  glm::vec3 position() const;
  glm::vec3 size() const;
  glm::vec3 min() const;
  glm::vec3 max() const;
  bool inside(const glm::vec3 &point) const;
};
}
}