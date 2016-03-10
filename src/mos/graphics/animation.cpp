#include <glm/glm.hpp>
#include <iostream>
#include <mos/graphics/animation.hpp>

namespace mos {

Animation::Animation(
    const std::map<unsigned int, std::shared_ptr<Mesh const>> keyframes,
    const unsigned int frame_rate)
    : keyframes_(keyframes.begin(), keyframes.end()),
      mesh_(std::make_shared<mos::Mesh>(*keyframes_.begin()->second)),
      time_(0.0f), frame_rate_(frame_rate) {
  std::cout << keyframes_.size() << std::endl;
  for (auto & p : keyframes_){
    std::cout << p.first << " " << p.second->vertices_size() << std::endl;
  }
  std::cout << mesh_->vertices_size() << std::endl << "----\n";
}

Animation::Animation(
    std::initializer_list<std::pair<unsigned int, std::shared_ptr<const Mesh>>>
        keyframes,
    const unsigned int frame_rate)
    : keyframes_(keyframes.begin(), keyframes.end()),
      mesh_(std::make_shared<mos::Mesh>(*keyframes_.begin()->second)),
      time_(0.0f), frame_rate_(frame_rate) {}

Animation::~Animation() {}

unsigned int Animation::frame() const {
  return glm::floor(time_ * frame_rate_);
}

void Animation::reset() { time_ = 0; }

void Animation::frame_rate(const unsigned int frame_rate) {
  frame_rate_ = frame_rate;
}

unsigned int Animation::frame_rate() const { return frame_rate_; }

void Animation::update(const float dt) {
  time_ += dt;
  if (frame() >= keyframes_.rbegin()->first) {
    time_ = 0;
  }

  std::cout << frame() << std::endl;
  auto next_frame = keyframes_.upper_bound(frame());
  auto previous_frame = next_frame;
  previous_frame--;
  // auto a =  (float)(frame() - previous_frame->first) /
  // (float)(next_frame->first - previous_frame->first);
  auto amount = (float)((time_ * frame_rate_) - previous_frame->first) /
                (float)(next_frame->first - previous_frame->first);

  mesh_->mix(*previous_frame->second, *next_frame->second, amount);

}

std::shared_ptr<Mesh> Animation::mesh() { return mesh_; }
}
