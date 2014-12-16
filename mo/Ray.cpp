#include "ray.h"

namespace mo {

Ray::Ray(const glm::vec3 & origin, const glm::vec3 & direction):
    origin(origin), direction(direction),
    inverse_direction(1.0f/direction.x, 1.0f/direction.y, 1.0f/direction.z)
{
}

}
