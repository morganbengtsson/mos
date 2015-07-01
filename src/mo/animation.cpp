#include <glm/glm.hpp>
#include <iostream>
#include "animation.hpp"

namespace mo {

Animation::Animation(std::map<unsigned int, std::shared_ptr<Mesh>> keyframes) :
    keyframes_(keyframes.begin(), keyframes.end()),
    mesh_(std::make_shared<mo::Mesh>(*keyframes_.begin()->second)),
    time_(0.0f) {
}

Animation::Animation(std::initializer_list<std::pair<unsigned int, std::shared_ptr<Mesh>>> keyframes):
keyframes_(keyframes.begin(), keyframes.end()),
mesh_(std::make_shared<mo::Mesh>(*keyframes_.begin()->second)),
time_(0.0f) {
}

Animation::~Animation() {
}

void Animation::update(const float dt) {
    time_ += dt;
    auto key_it = keyframes_.upper_bound((unsigned int)time_);

    auto key1 = keyframes_[1];
    auto key2 = keyframes_[5];

    auto key1_it = key1->vertices_begin();
    auto key2_it = key2->vertices_begin();
    for (auto vertex_it = mesh_->vertices_begin(); vertex_it != mesh_->vertices_end(); vertex_it++) {
        //*vertex_it = *key2_it;
        vertex_it->position = glm::mix(key1_it->position, key2_it->position, time_ / 10.0f);

        key1_it++;
        key2_it++;
    }

    /*
    auto k_it = key_it->second->vertices_begin();
    for (auto v_it = mesh_->vertices_begin(); v_it != mesh_->vertices_end(); v_it++) {
        *v_it = *k_it;
        k_it++;
    }
    */
    mesh_->valid = false;
}

std::shared_ptr<Mesh> Animation::mesh() {
    return mesh_;
}

}

