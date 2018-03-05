#pragma once

#include <memory>
#include <atomic>
#include <experimental/optional>
#include <glm/glm.hpp>
#include <mos/render/texture_2d.hpp>
#include <mos/render/texture_cube.hpp>

namespace mos {
namespace gfx {
class Target;
using OptTarget = std::experimental::optional<Target>;
/**
 * @brief The RenderTarget class
 *
 * Represents a rendertarget, with asociated texture. For off screen rendering.
 */
class Target {
public:

  /**
   * @brief Target
   * @param resolution
   */
  Target();

  /**
   * @brief unique id
   * @return id
   */
  unsigned int id() const;

private:
  unsigned int id_;
  static std::atomic_uint current_id_;
};
}
}