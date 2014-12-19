#ifndef MO_BOX_HPP
#define MO_BOX_HPP

#include <glm/glm.hpp>
#include "Ray.h"

namespace mo
{

class Box
{
public:
    Box();
    Box(const glm::vec3 & min, const glm::vec3 & max, const glm::mat4 & transform);
    bool intersect(const Ray &, float t0, float t1) const;
    glm::vec3 parameters[2];
    glm::mat4 transform;
private:


};
}
#endif // MO_BOX_HPP
