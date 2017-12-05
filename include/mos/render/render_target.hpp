#ifndef MOS_RENDER_TARGET_HPP
#define MOS_RENDER_TARGET_HPP

#include <memory>
#include <atomic>
#include <optional.hpp>
#include <glm/glm.hpp>
#include <mos/render/texture_2d.hpp>
#include <mos/render/texture_cube.hpp>

namespace mos {
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
#endif // MOS_RENDER_TARGET_HPP
