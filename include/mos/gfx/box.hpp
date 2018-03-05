#pragma once

#include <glm/glm.hpp>

namespace mos {
namespace gfx {

/** Bounding box for rendering. */
class Box final {
public:
  Box(const glm::vec3 &position,
      const glm::vec3 &extent);
  glm::vec3 position;
  glm::vec3 extent;
  glm::vec3 size() const;
  glm::vec3 min() const;
  glm::vec3 max() const;
};
}
}