#include <glm/glm.hpp>
#include <iostream>
#include <json.hpp>
#include <mos/util.hpp>
#include <mos/gfx/animation.hpp>
#include <filesystem/path.h>

namespace mos {
namespace gfx {
using namespace nlohmann;
Animation::Animation(
    const std::map<unsigned int, std::shared_ptr<Mesh const>> &keyframes,
    const unsigned int frame_rate)
    : frame_rate_(frame_rate),
      mesh_(std::make_shared<Mesh>(*keyframes_.begin()->second)),
      keyframes_(keyframes.begin(), keyframes.end()) {
  std::cout << keyframes_.size() << std::endl;
  for (auto &p : keyframes_) {
    std::cout << p.first << " " << p.second->vertices.size() << std::endl;
  }
  std::cout << mesh_->vertices.size() << std::endl << "----\n";
}

Animation::Animation(
    std::initializer_list<std::pair<unsigned int, std::shared_ptr<const Mesh>>>
        keyframes,
    const unsigned int frame_rate)
    : frame_rate_(frame_rate),
      mesh_(std::make_shared<Mesh>(*keyframes_.begin()->second)),
      keyframes_(keyframes.begin(), keyframes.end()) {}

Animation::Animation(const std::string &path) {
  filesystem::path fpath = path;
  auto doc = json::parse(mos::text(fpath.str()));
  frame_rate_ = doc["frame_rate"];
  for (auto &keyframe : doc["keyframes"]) {
    auto key = keyframe["key"];
    std::string mesh_file = keyframe["mesh"];
    keyframes_.insert({key, Mesh::load(fpath.parent_path().str() + "/" + mesh_file)});
  }
}

Animation::Animation(Assets &assets, const std::string &path) {
  auto doc = json::parse(mos::text(assets.directory() + path));
  frame_rate_ = doc["frame_rate"];
  for (auto &keyframe : doc["keyframes"]) {
    auto key = keyframe["key"];
    auto mesh_path = keyframe["mesh"];
    keyframes_.insert({key, assets.mesh(mesh_path)});
  }
}

int Animation::frame() const {
  return int(glm::floor(time_ * frame_rate_));
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
  auto next_frame = keyframes_.upper_bound(frame());
  auto previous_frame = next_frame;
  previous_frame--;
  auto amount = static_cast<float>((time_ * frame_rate_) - previous_frame->first) /
      static_cast<float>(next_frame->first - previous_frame->first);

  mesh_->mix(*previous_frame->second, *next_frame->second, amount);
}

std::shared_ptr<Mesh> Animation::mesh() { return mesh_; }


}
}
