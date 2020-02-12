#pragma once
#include <glm/glm.hpp>

namespace mos {
namespace gfx {

/** Particle used in a particle cloud. */
class Point final {
public:
  explicit Point(const glm::vec3 &position = glm::vec3(0.0f),
                    const glm::vec3 &color = glm::vec3(1.0f),
                    const float size = 10.0f,
                    const float alpha = 1.0f)
      : position(position), color(color), size(size), alpha(alpha) {}
  glm::vec3 position;
  glm::vec3 color;
  float size;
  float alpha;
};
}
}

