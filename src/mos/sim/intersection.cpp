#include <mos/sim/intersection.hpp>

namespace mos {
namespace sim {
Intersection::Intersection(const glm::vec3 &position, const glm::vec3 &normal)
    : position(position), normal(normal) {}

Intersection::~Intersection() {}
}
}