#include "box.hpp"

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <array>
#include <glm/gtx/io.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mo {

Box::Box(){
}

Box::Box(const glm::vec3 & min, const glm::vec3 & max, const glm::mat4 & transform) :
    transform_(transform), min_(min), max_(max) {
    transform_ = glm::translate(glm::mat4(1.0f), glm::vec3(transform_[3][0], transform_[3][1], transform_[3][2]));
    if(glm::all(glm::lessThan(max_, min_))){
        throw std::invalid_argument("Min must be less than max.");
    }
}

glm::vec3 Box::min() const {
    return (glm::vec3)(transform() * glm::vec4(min_, 1.0f));
}
glm::vec3 Box::max() const {
    return (glm::vec3)(transform() * glm::vec4(max_, 1.0f));
}

RayIntersection Box::intersect(const glm::vec3 & origin, const glm::vec3 direction, float t1, float t2) {
    glm::vec3 p1 = origin + (direction * t1);
    glm::vec3 p2 = origin + (direction * t2);
    return intersect(p1, p2);
}

RayIntersection Box::intersect(glm::vec3 point1, glm::vec3 point2) {
    glm::vec3 hit(0.0f);
    RayIntersection false_intersection{false, hit};
    glm::vec3 min = this->min();
    glm::vec3 max = this->max();
    if (point2.x < min.x && point1.x < min.x) {return false_intersection;}
    if (point2.x > max.x && point1.x > max.x) {return false_intersection;}
    if (point2.y < min.y && point1.y < min.y) {return false_intersection;}
    if (point2.y > max.y && point1.y > max.y) {return false_intersection;}
    if (point2.z < min.z && point1.z < min.z) {return false_intersection;}
    if (point2.z > max.z && point1.z > max.z) {return false_intersection;}
    if (point1.x > min.x &&
            point1.x < max.x &&
            point1.y > min.y &&
            point1.y < max.y &&
            point1.z > min.z &&
            point1.z < max.z) {
        hit = point1;
        return RayIntersection{true, hit};
    }
    if ( (intersection( point1.x-min.x, point2.x-min.x, point1, point2, hit) && in_box( hit, min, max, 1 ))
         || (intersection( point1.y-min.y, point2.y-min.y, point1, point2, hit) && in_box( hit, min, max, 2 ))
         || (intersection( point1.z-min.z, point2.z-min.z, point1, point2, hit) && in_box( hit, min, max, 3 ))
         || (intersection( point1.x-max.x, point2.x-max.x, point1, point2, hit) && in_box( hit, min, max, 1 ))
         || (intersection( point1.y-max.y, point2.y-max.y, point1, point2, hit) && in_box( hit, min, max, 2 ))
         || (intersection( point1.z-max.z, point2.z-max.z, point1, point2, hit) && in_box( hit, min, max, 3 ))) {
        return RayIntersection{true, hit};
    }
    return false_intersection;
}

BoxIntersection Box::intersects(const Box &other) {
    static const std::array<glm::vec3, 6> faces = {
        glm::vec3(-1, 0, 0), // 'left' face normal (-x direction)
        glm::vec3( 1, 0, 0), // 'right' face normal (+x direction)
        glm::vec3( 0,-1, 0), // 'bottom' face normal (-y direction)
        glm::vec3( 0, 1, 0), // 'top' face normal (+y direction)
        glm::vec3( 0, 0,-1), // 'far' face normal (-z direction)
        glm::vec3( 0, 0, 1), // 'near' face normal (+x direction)
    };

    glm::vec3 maxa = this->max();
    glm::vec3 mina = this->min();
    glm::vec3 maxb = other.max();
    glm::vec3 minb = other.min();

    std::array<float, 6> distances = {
        (maxb.x - mina.x), // distance of box 'b' to face on 'left' side of 'a'.
        (maxa.x - minb.x), // distance of box 'b' to face on 'right' side of 'a'.
        (maxb.y - mina.y), // distance of box 'b' to face on 'bottom' side of 'a'.
        (maxa.y - minb.y), // distance of box 'b' to face on 'top' side of 'a'.
        (maxb.z - mina.z), // distance of box 'b' to face on 'far' side of 'a'.
        (maxa.z - minb.z), // distance of box 'b' to face on 'near' side of 'a'.
    };

    glm::vec3 normal(0.0f);
    float distance = 0.0f;
    for(int i = 0; i < 6; i ++) {
            // box does not intersect face. So boxes don't intersect at all.
            if(distances[i] < 0.0f){
                return BoxIntersection{false, glm::vec3(0.0f), distance};
            }
            // face of least intersection depth. That's our candidate.
            if((i == 0) || (distances[i] < distance))
            {
                //fcoll = i;
                normal = faces[i];
                distance = distances[i];
            }
        }
    return BoxIntersection{true, normal, distance};
}

glm::mat4 Box::transform() const
{
    return transform_;
}

void Box::transform(const glm::mat4 &transform)
{
    transform_ = transform;
}

}
