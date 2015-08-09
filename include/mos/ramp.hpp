#ifndef MOS_RAMP_HPP
#define MOS_RAMP_HPP

#include <glm/glm.hpp>
#include <tuple>
#include "box.hpp"

namespace mos {

class Ramp {
public:
    template<class It>
    Ramp(It begin,
         It end,
         const glm::mat4 & transform){
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

    ~Ramp();

    std::tuple<glm::vec3, float> intersects(const Box & box);
    glm::mat4 transform;
    glm::vec3 min() const;
    glm::vec3 max() const;

private:
    glm::vec3 min_;
    glm::vec3 max_;
};

}

#endif // MOS_RAMP_HPP
