#pragma once

#include <mos/gfx/mesh.hpp>
#include <initializer_list>
#include <map>
#include <memory>
#include <utility>

namespace mos {
namespace gfx {

/** Keyframe animation, interpolation between meshes. */
class Animation final {
public:
  Animation();
  Animation(const std::map<unsigned int, std::shared_ptr<Mesh const>> keyframes,
            const unsigned int frame_rate = 30);
   Animation(std::initializer_list<
      std::pair<unsigned int, std::shared_ptr<Mesh const>>>
            keyframes,
            const unsigned int frame_rate = 30);

  ~Animation();

  Animation(const std::string &path);

  void update(const float dt);

  /** Current frame */
  unsigned int frame() const;

  /** Restart the animation. */
  void reset();

  /** Set frame rate. */
  void frame_rate(const unsigned int frame_rate);

  /** Frames per second. */
  unsigned int frame_rate() const;

  /** The mesh being animated. */
  std::shared_ptr<Mesh> mesh();

private:
  float time_;
  std::map<unsigned int, std::shared_ptr<const Mesh>> keyframes_;
  std::shared_ptr<Mesh> mesh_;
  unsigned int frame_rate_;
};
}
}
