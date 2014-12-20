#ifndef MO_BOX_HPP
#define MO_BOX_HPP

#include <array>
#include <glm/glm.hpp>
#include <utility>
#include <algorithm>
#include "ray.hpp"
#include "vertex.hpp"

namespace mo
{

class Box
{
public:
    template<class VertexIt>
    Box(VertexIt begin, VertexIt end, const glm::mat4 & transform) : transform(transform){
        glm::vec3 min, max;

        if (begin != end){
            auto x_extremes = std::minmax_element(begin, end,
                                                  [](const Vertex& left, const Vertex& right) {
                return left.position.x < right.position.x;
            });

            auto y_extremes = std::minmax_element(begin, end,
                                                  [](const Vertex& left, const Vertex& right) {
                return left.position.y < right.position.y;
            });

            auto z_extremes = std::minmax_element(begin, end,
                                                  [](const Vertex& left, const Vertex& right) {
                return left.position.z < right.position.z;
            });

            min = glm::vec3(x_extremes.first->position.x, y_extremes.first->position.y, z_extremes.first->position.z);
            max = glm::vec3(x_extremes.second->position.x, y_extremes.second->position.y, z_extremes.second->position.z);
        }
        parameters[0] = (glm::vec3)(transform * glm::vec4(min, 1.0f));
        parameters[1] = (glm::vec3)(transform * glm::vec4(max, 1.0f));
    }

    Box();
    Box(const glm::vec3 & min, const glm::vec3 & max, const glm::mat4 & transform);
    bool intersect(const Ray &, float t0, float t1) const;
    glm::vec3 parameters[2];
    glm::mat4 transform;
    std::array<glm::vec3, 8> positions() const;

    bool intersect(glm::vec3 L1, glm::vec3 L2, glm::vec3 &Hit)
    {
        glm::vec3 B1 = parameters[0];
        glm::vec3 B2 = parameters[1];
        if (L2.x < B1.x && L1.x < B1.x) return false;
        if (L2.x > B2.x && L1.x > B2.x) return false;
        if (L2.y < B1.y && L1.y < B1.y) return false;
        if (L2.y > B2.y && L1.y > B2.y) return false;
        if (L2.z < B1.z && L1.z < B1.z) return false;
        if (L2.z > B2.z && L1.z > B2.z) return false;
        if (L1.x > B1.x && L1.x < B2.x &&
                L1.y > B1.y && L1.y < B2.y &&
                L1.z > B1.z && L1.z < B2.z)
        {Hit = L1;
            return true;}
        if ( (GetIntersection( L1.x-B1.x, L2.x-B1.x, L1, L2, Hit) && InBox( Hit, B1, B2, 1 ))
             || (GetIntersection( L1.y-B1.y, L2.y-B1.y, L1, L2, Hit) && InBox( Hit, B1, B2, 2 ))
             || (GetIntersection( L1.z-B1.z, L2.z-B1.z, L1, L2, Hit) && InBox( Hit, B1, B2, 3 ))
             || (GetIntersection( L1.x-B2.x, L2.x-B2.x, L1, L2, Hit) && InBox( Hit, B1, B2, 1 ))
             || (GetIntersection( L1.y-B2.y, L2.y-B2.y, L1, L2, Hit) && InBox( Hit, B1, B2, 2 ))
             || (GetIntersection( L1.z-B2.z, L2.z-B2.z, L1, L2, Hit) && InBox( Hit, B1, B2, 3 )))
            return true;

        return false;
    }

private:
    int inline GetIntersection( float fDst1, float fDst2, glm::vec3 P1, glm::vec3 P2, glm::vec3 &Hit) {
        if ( (fDst1 * fDst2) >= 0.0f) return 0;
        if ( fDst1 == fDst2) return 0;
        Hit = P1 + (P2-P1) * ( -fDst1/(fDst2-fDst1) );
        return 1;
    }

    int inline InBox( glm::vec3 Hit, glm::vec3 B1, glm::vec3 B2, const int Axis) {
        if ( Axis==1 && Hit.z > B1.z && Hit.z < B2.z && Hit.y > B1.y && Hit.y < B2.y) return 1;
        if ( Axis==2 && Hit.z > B1.z && Hit.z < B2.z && Hit.x > B1.x && Hit.x < B2.x) return 1;
        if ( Axis==3 && Hit.x > B1.x && Hit.x < B2.x && Hit.y > B1.y && Hit.y < B2.y) return 1;
        return 0;
    }

};
}
#endif // MO_BOX_HPP
