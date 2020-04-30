#pragma once

#include <initializer_list>
#include <map>
#include <memory>
#include <utility>
#include <mos/gfx/mesh.hpp>
#include <mos/gfx/assets.hpp>

namespace mos::gfx {

/** Keyframe animation, interpolation between meshes. */
class Animation final {
public:
  Animation(const std::map<unsigned int, std::shared_ptr<Mesh const>> &keyframes,
            unsigned int frame_rate = 30);
  Animation(std::initializer_list<
      std::pair<unsigned int, std::shared_ptr<Mesh const>>>
            keyframes,
            unsigned int frame_rate = 30);

  Animation(const std::string &path);

  Animation(Assets &assets, const std::string &path);

  /** Step the animation forward. */
  auto step(float dt) -> void;

  /** Current frame */
  auto frame() const -> int;

  /** Restart the animation. */
  auto reset() -> void;

  /** Set frame rate. */
  auto frame_rate(unsigned int frame_rate) -> void;

  /** Frames per second. */
  auto frame_rate() const -> unsigned int;

  /** The mesh being animated. */
  std::shared_ptr<Mesh> mesh();

private:
  float time_{0.0f};
  unsigned int frame_rate_{30};
  std::shared_ptr<Mesh> mesh_;
  std::map<int, std::shared_ptr<const Mesh>> keyframes_;
  };
}

