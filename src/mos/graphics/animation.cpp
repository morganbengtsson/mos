#include <glm/glm.hpp>
#include <iostream>
#include "graphics/animation.hpp"

namespace mos {

Animation::Animation(std::initializer_list<std::pair<unsigned int, std::shared_ptr<const Mesh>>> keyframes):
keyframes_(keyframes.begin(), keyframes.end()),
mesh_(std::make_shared<mos::Mesh>(*keyframes_.begin()->second)),
time_(0.0f), frames_per_second_(30), transition_index_(-1){
}

Animation::~Animation() {
}

int Animation::frame() const {
    return glm::floor(time_ * frames_per_second_);
}

void Animation::reset() {
    time_ = 0;
}

void Animation::frames_per_second(const int frames_per_second) {
    frames_per_second_  = frames_per_second;
}

int Animation::frames_per_second() const {
    return frames_per_second_;
}

void Animation::update(const float dt) {
    time_ += dt;

    auto next_frame = keyframes_.upper_bound(frame());
    auto previous_frame = next_frame;
    previous_frame--;
    //auto a =  (float)(frame() - previous_frame->first) / (float)(next_frame->first - previous_frame->first);
    auto a = (float)((time_ * frames_per_second_) - previous_frame->first) / (float)(next_frame->first - previous_frame->first);
    std::cout << "---" << std::endl;
    std::cout << "l: " << previous_frame->first << std::endl;
    std::cout << frame() << std::endl;
    std::cout << "h: "  << next_frame->first << std::endl;
    std::cout << "a: " << a << std::endl;

    if (frame() >= keyframes_.rbegin()->first){
            time_ = 0;
    }
    auto vertices1_it = previous_frame->second->vertices_begin();
    if (previous_frame->first == 0 && transition != nullptr){
        vertices1_it = transition->vertices_begin();
    }
    else {
        transition = nullptr;
    }
    auto vertices2_it = next_frame->second->vertices_begin();
    for (auto vertex_it = mesh_->vertices_begin(); vertex_it != mesh_->vertices_end(); vertex_it++) {

        vertex_it->position = glm::mix(vertices1_it->position, vertices2_it->position, a);

        vertices1_it++;
        vertices2_it++;
    }
    mesh_->invalidate();
}

std::shared_ptr<Mesh> Animation::mesh() {
    return mesh_;
}

}

