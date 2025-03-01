#pragma once

#include <glm/glm.hpp>

namespace mos::gfx {

/** Particle used in a particle cloud. */
class Point final {
public:
  explicit Point(const glm::vec3 &position = glm::vec3(0.0F),
                 const glm::vec3 &color = glm::vec3(1.0F),
                 const float size = 10.0F, const float alpha = 1.0F)
      : position(position), color(color), size(size), alpha(alpha) {}
  glm::vec3 position{0.0F};
  glm::vec3 color{1.0F};
  float size{10.0F};
  float alpha{1.0F};
};
} // namespace mos::gfx
