#pragma once

#include <memory>
#include <atomic>
#include <experimental/optional>
#include <glm/glm.hpp>
#include <mos/render/texture_2d.hpp>
#include <mos/render/texture_cube.hpp>

namespace mos {
namespace gfx {
class RenderTarget;
using OptTarget = std::experimental::optional<RenderTarget>;
/**
 * @brief The RenderTarget class
 *
 * Represents a rendertarget, with asociated texture. For off screen rendering.
 */
class RenderTarget {
public:

  /**
   * @brief Target
   * @param resolution
   */
  RenderTarget();

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