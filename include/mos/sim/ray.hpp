#pragma once
#include <glm/glm.hpp>

namespace mos::sim {

/** Ray for ray testing. */
class Ray {
public:
  Ray(const glm::vec3 &origin,
      const glm::vec3 &direction);
  glm::vec3 origin;
  auto direction() const -> glm::vec3;
  auto direction(const glm::vec3 &direction) -> void;
private:
  glm::vec3 direction_;
};
}

