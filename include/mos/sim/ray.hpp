#pragma once
#include <glm/glm.hpp>

namespace mos {
namespace sim {

/** Ray for ray testing. */
class Ray {
public:
  Ray(const glm::vec3 &origin,
      const glm::vec3 &direction);
  glm::vec3 origin;
  glm::vec3 direction() const;
  void direction(const glm::vec3 &direction);
private:
  glm::vec3 direction_;
};
}
}