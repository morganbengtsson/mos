#include <mos/sim/ray.hpp>

namespace mos::sim {
Ray::Ray(const glm::vec3 &origin, const glm::vec3 &direction)
    : origin(origin), direction_(glm::normalize(direction)) {}

auto Ray::direction() const -> glm::vec3 { return direction_; }

void Ray::direction(const glm::vec3 &direction) {
  direction_ = glm::normalize(direction);
}
} // namespace mos::sim
