#include "ray.hpp"
#include <glm/gtx/io.hpp>
#include <iostream>
namespace mo {

Ray::Ray(const glm::vec3 & origin, const glm::vec3 & direction):
    origin(origin), direction(direction)
    ,inverse_direction(1.0f/direction.x, 1.0f/direction.y, 1.0f/direction.z)
{
    std::cout << origin << std::endl;
    std::cout << direction << std::endl;
    std::cout << inverse_direction << std::endl;
    sign[0] = (inverse_direction.x < 0);
    sign[1] = (inverse_direction.y < 0);
    sign[2] = (inverse_direction.z < 0);
}

}
