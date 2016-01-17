#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <glm/glm.hpp>

class Intersection
{
public:
    Intersection(const glm::vec3 & position,
                 const glm::vec3 & normal);
    ~Intersection();
    glm::vec3 position;
    glm::vec3 normal;
};

#endif // INTERSECTION_HPP
