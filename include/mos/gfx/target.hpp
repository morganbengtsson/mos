#pragma once
#include <memory>
#include <atomic>
#include <optional>
#include <glm/glm.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/texture_cube.hpp>

namespace mos {
namespace gfx {
class Target;
using OptTarget = std::optional<Target>;

/** Rendertarget,for off screen rendering. */
class Target {
public:
  Target();

  /** Unique id */
  unsigned int id() const;
private:
  unsigned int id_;
  static std::atomic_uint current_id_;
};
}
}