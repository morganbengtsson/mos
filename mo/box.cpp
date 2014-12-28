#include "box.hpp"

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <glm/gtx/io.hpp>
#include <glm/glm.hpp>

namespace mo {

Box::Box(){
}

Box::Box(const glm::vec3 & min, const glm::vec3 & max, const glm::mat4 & transform) :
    transform(transform), min_(min), max_(max) {
    if(glm::all(glm::lessThan(max_, min_))){
        throw std::invalid_argument("Min must be less than max.");
    }
    assert(min_ < max_);
}

glm::vec3 Box::min() const {
    return (glm::vec3)(transform * glm::vec4(min_, 1.0f));
}
glm::vec3 Box::max() const {
    return (glm::vec3)(transform * glm::vec4(max_, 1.0f));
}

std::pair<bool, glm::vec3> Box::intersect(const glm::vec3 & origin, const glm::vec3 direction, float t1, float t2) {
    glm::vec3 p1 = origin + (direction * t1);
    glm::vec3 p2 = origin + (direction * t2);
    return intersect(p1, p2);
}

bool Box::intersects(const Box &other) {
    glm::vec3 min = this->min();
    glm::vec3 max = this->max();
    glm::vec3 other_min = other.min();
    glm::vec3 other_max = other.max();
    return(max.x > other_min.x &&
        min.x < other_max.x &&
        max.y > other_min.y &&
        min.y < other_max.y &&
        max.z > other_min.z &&
        min.z < other_max.z);
}

std::pair<bool, glm::vec3> Box::intersect(glm::vec3 point1, glm::vec3 point2) {
    glm::vec3 hit(0.0f);
    std::pair<bool, glm::vec3> false_pair(false, hit);
    glm::vec3 min = this->min();
    glm::vec3 max = this->max();
    if (point2.x < min.x && point1.x < min.x) {return false_pair;}
    if (point2.x > max.x && point1.x > max.x) {return false_pair;}
    if (point2.y < min.y && point1.y < min.y) {return false_pair;}
    if (point2.y > max.y && point1.y > max.y) {return false_pair;}
    if (point2.z < min.z && point1.z < min.z) {return false_pair;}
    if (point2.z > max.z && point1.z > max.z) {return false_pair;}
    if (point1.x > min.x &&
            point1.x < max.x &&
            point1.y > min.y &&
            point1.y < max.y &&
            point1.z > min.z &&
            point1.z < max.z) {
        hit = point1;
        return std::pair<bool, glm::vec3>(true, hit);
    }
    if ( (intersection( point1.x-min.x, point2.x-min.x, point1, point2, hit) && in_box( hit, min, max, 1 ))
         || (intersection( point1.y-min.y, point2.y-min.y, point1, point2, hit) && in_box( hit, min, max, 2 ))
         || (intersection( point1.z-min.z, point2.z-min.z, point1, point2, hit) && in_box( hit, min, max, 3 ))
         || (intersection( point1.x-max.x, point2.x-max.x, point1, point2, hit) && in_box( hit, min, max, 1 ))
         || (intersection( point1.y-max.y, point2.y-max.y, point1, point2, hit) && in_box( hit, min, max, 2 ))
         || (intersection( point1.z-max.z, point2.z-max.z, point1, point2, hit) && in_box( hit, min, max, 3 ))) {
        return std::pair<bool, glm::vec3>(true, hit);
    }
    return false_pair;
}

}
