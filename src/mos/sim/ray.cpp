#include <mos/sim/ray.hpp>

namespace mos {
namespace sim {
Ray::Ray(const glm::vec3 &origin, const glm::vec3 &direction)
    : origin(origin), direction_(glm::normalize(direction)) {}

glm::vec3 Ray::direction() const { return direction_; }

void Ray::direction(const glm::vec3 &direction) {
  direction_ = glm::normalize(direction);
}
}
}
