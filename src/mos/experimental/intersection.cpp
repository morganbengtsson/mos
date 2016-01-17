#include <mos/experimental/intersection.hpp>

Intersection::Intersection(const glm::vec3 & position,
                           const glm::vec3 & normal) :
    position(position),
    normal(normal) {
}

Intersection::~Intersection() {
}

