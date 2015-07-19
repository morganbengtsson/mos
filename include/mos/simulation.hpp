#include <glm/glm.hpp>
#include "model.hpp"

namespace mos {

template<class It>
void selections(It begin, It end, const glm::vec3 & position, glm::vec3 & direction){
    mos::Model * closest = nullptr;
    float distance = 10000.0f;
    for (auto it = begin; it != end; it++){
        auto & model = *it;
        auto res = model.box.intersect(position, glm::normalize(direction), 0.0f, 1000.0f);
        model.selected(false);
        if (res.intersects){
            if (res.distance < distance){
                distance = res.distance;
                closest = &model;
            }
        }
    }
    if (closest != nullptr){
        closest->selected(true);
    }
}

}
