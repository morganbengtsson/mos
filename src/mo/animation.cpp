#include <glm/glm.hpp>
#include <iostream>
#include "animation.hpp"

namespace mo {

Animation::Animation(std::map<unsigned int, std::shared_ptr<Mesh>> keyframes) :
    keyframes_(keyframes.begin(), keyframes.end()),
    mesh_(keyframes_.begin()->second),
    time_(0.0f) {
}

Animation::Animation(std::initializer_list<std::pair<unsigned int, std::shared_ptr<Mesh>>> keyframes):
keyframes_(keyframes.begin(), keyframes.end()),
mesh_(keyframes_.begin()->second),
time_(0.0f) {
}

Animation::~Animation() {
}

void Animation::update(const float dt) {
    time_ += dt;
    auto key_it = keyframes_.lower_bound((unsigned int)time_);
    auto key2_it = keyframes_.upper_bound((unsigned int)time_);

    std::cout << key_it->first << std::endl;
    std::cout << time_ << std::endl;
    std::cout << key2_it->first << std::endl << "---" << std::endl;
    auto k_it = key_it->second->vertices_begin();
    for (auto v_it = mesh_->vertices_begin(); v_it != mesh_->vertices_end(); v_it++) {
        *v_it = *k_it;
        k_it++;
    }
    mesh_->valid = false;
}

}

