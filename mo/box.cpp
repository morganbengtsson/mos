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

}
