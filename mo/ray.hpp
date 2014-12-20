#ifndef RAY_H
#define RAY_H

#include <glm/vec3.hpp>

namespace mo {

class Ray
{
public:
    Ray(const glm::vec3 & origin, const glm::vec3 & direction);
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 inverse_direction;
    int sign[3];
};

}
#endif // RAY_H
