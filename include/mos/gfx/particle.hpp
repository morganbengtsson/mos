#pragma once
#include <glm/glm.hpp>

namespace mos {
namespace gfx {

/** Particle used in a particle system. */
class Particle final {
public:
  explicit Particle(const glm::vec3 &position = glm::vec3(0.0f),
                    const glm::vec4 &color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                    const float size = 10.0f, const float opacity = 1.0f)
      : position(position), color(color), size(size), opacity(opacity) {}

  ~Particle() {}

  glm::vec3 position;

  glm::vec4 color;

  float size;

  float opacity;
};
}
}

