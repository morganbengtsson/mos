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
    Box(VertexIt begin, VertexIt end, const glm::mat4 & transform) : transform(transform) {
        glm::vec3 min, max;

        if (begin != end) {
            auto x_extremes = std::minmax_element(begin, end, [](const Vertex& left, const Vertex& right) {
                return left.position.x < right.position.x;
            });

            auto y_extremes = std::minmax_element(begin, end, [](const Vertex& left, const Vertex& right) {
                return left.position.y < right.position.y;
            });

            auto z_extremes = std::minmax_element(begin, end, [](const Vertex& left, const Vertex& right) {
                return left.position.z < right.position.z;
            });

            min = glm::vec3(x_extremes.first->position.x, y_extremes.first->position.y, z_extremes.first->position.z);
            max = glm::vec3(x_extremes.second->position.x, y_extremes.second->position.y, z_extremes.second->position.z);
        }
        min_ = min;
        max_ = max;
    }

    Box();
    Box(const glm::vec3 & min, const glm::vec3 & max, const glm::mat4 & transform);
    glm::vec3 min() const;
    glm::vec3 max() const;
    std::pair<bool, glm::vec3> intersect(glm::vec3 point1, glm::vec3 point2);
    std::pair<bool, glm::vec3> intersect(const glm::vec3 & origin, const glm::vec3 direction, float t1, float t2);
    std::pair<bool, glm::vec3> intersects(const Box & other);
    glm::mat4 transform;
private:
    glm::vec3 min_;
    glm::vec3 max_;

    bool inline intersection( float fDst1, float fDst2, glm::vec3 point1, glm::vec3 point2, glm::vec3 &hit) {
        if ( (fDst1 * fDst2) >= 0.0f) return false;
        if ( fDst1 == fDst2) return false;
        hit = point1 + (point2-point1) * ( -fDst1/(fDst2-fDst1) );
        return true;
    }

    bool inline in_box( glm::vec3 Hit, glm::vec3 min, glm::vec3 max, const int axis) {
        if ( axis==1 && Hit.z > min.z && Hit.z < max.z && Hit.y > min.y && Hit.y < max.y) { return true; }
        if ( axis==2 && Hit.z > min.z && Hit.z < max.z && Hit.x > min.x && Hit.x < max.x) { return true; }
        if ( axis==3 && Hit.x > min.x && Hit.x < max.x && Hit.y > min.y && Hit.y < max.y) { return true; }
        return false;
    }

};
}
#endif // MO_BOX_HPP
