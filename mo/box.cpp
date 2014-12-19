#include "box.hpp"

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <glm/gtx/io.hpp>
#include <glm/glm.hpp>

namespace mo {

Box::Box(){
}

Box::Box(const glm::vec3 & min, const glm::vec3 & max, const glm::mat4 & transform) : transform(transform) {
    if(glm::all(glm::lessThan(max, min))){
        throw std::invalid_argument("Min must be less than max.");
    }
    assert(min < max);
    parameters[0] = (glm::vec3)(transform * glm::vec4(min, 1.0f));
    parameters[1] = (glm::vec3)(transform * glm::vec4(max, 1.0f));
}

bool Box::intersect(const Ray &r, float t0, float t1) const {
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    tmin = (parameters[r.sign[0]].x - r.origin.x) * r.inverse_direction.x;
    tmax = (parameters[1-r.sign[0]].x - r.origin.x) * r.inverse_direction.x;
    tymin = (parameters[r.sign[1]].y - r.origin.y) * r.inverse_direction.y;
    tymax = (parameters[1-r.sign[1]].y - r.origin.y) * r.inverse_direction.y;
    if ( (tmin > tymax) || (tymin > tmax) )
        return false;
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;
    tzmin = (parameters[r.sign[2]].z - r.origin.z) * r.inverse_direction.z;
    tzmax = (parameters[1-r.sign[2]].z - r.origin.z) * r.inverse_direction.z;
    if ( (tmin > tzmax) || (tzmin > tmax) )
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;
    return ( (tmin < t1) && (tmax > t0) );
}

}
