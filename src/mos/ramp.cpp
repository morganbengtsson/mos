#include "ramp.hpp"

namespace mos {

Ramp::~Ramp() {
}

std::tuple<glm::vec3, float> Ramp::intersects(const Box & other) {
    static const std::array<glm::vec3, 6> faces = {
        glm::vec3(-1, 0, 0), // 'left' face normal (-x direction)
        glm::vec3( 1, 0, 0), // 'right' face normal (+x direction)
        glm::vec3( 0,-1, 0), // 'bottom' face normal (-y direction)
        glm::vec3( 0, 1, 0), // 'top' face normal (+y direction)
        glm::vec3( 0, 0,-1), // 'far' face normal (-z direction)
        glm::vec3( 0, 0, 1), // 'near' face normal (+z direction)
    };

    glm::vec3 maxa = this->max();
    glm::vec3 mina = this->min();

    //TODO: This swapping is kind of nasty. Look throgh the whol BB thing.
    if (maxa.x < mina.x){std::swap(maxa.x, mina.x);}
    if (maxa.y < mina.y){std::swap(maxa.y, mina.y);}
    if (maxa.z < mina.z){std::swap(maxa.z, mina.z);}

    glm::vec3 maxb = other.max();
    glm::vec3 minb = other.min();


    if (maxb.x < minb.x){std::swap(maxb.x, minb.x);}
    if (maxb.y < minb.y){std::swap(maxb.y, minb.y);}
    if (maxb.z < minb.z){std::swap(maxb.z, minb.z);}

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
            if(distances[i] < 0.0f) {
                return std::tuple<glm::vec3, float>(glm::vec3(0.0f), distance);
            }
            // face of least intersection depth. That's our candidate.
            if((i == 0) || (distances[i] < distance))
            {
                //fcoll = i;
                normal = faces[i];
                distance = distances[i];
            }
        }
    return std::tuple<glm::vec3, float>(normal, distance);
}

glm::vec3 Ramp::min() const {
    return (glm::vec3)(transform * glm::vec4(min_, 1.0f));
}
glm::vec3 Ramp::max() const {
    return (glm::vec3)(transform * glm::vec4(max_, 1.0f));
}



}
