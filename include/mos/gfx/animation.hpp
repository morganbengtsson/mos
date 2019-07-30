#pragma once

#include <initializer_list>
#include <map>
#include <memory>
#include <utility>
#include <mos/gfx/mesh.hpp>
#include <mos/gfx/assets.hpp>

namespace mos {
namespace gfx {

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

  void update(float dt);

  /** Current frame */
  int frame() const;

  /** Restart the animation. */
  void reset();

  /** Set frame rate. */
  void frame_rate(unsigned int frame_rate);

  /** Frames per second. */
  unsigned int frame_rate() const;

  /** The mesh being animated. */
  std::shared_ptr<Mesh> mesh();

private:
  float time_{0.0f};
  unsigned int frame_rate_{30};
  std::shared_ptr<Mesh> mesh_;
  std::map<int, std::shared_ptr<const Mesh>> keyframes_;
  };
}
}
