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
  explicit RenderTarget(const SharedTexture2D & texture,
                        const SharedTexture2D & depth_texture,
                        const SharedTextureCube texture_cube);

  /**
   * @brief The texture that is rendered to.
   */
  const SharedTexture2D texture;

  /**
   * @brief The depth texture that is rendered to.
   */
  const SharedTexture2D depth_texture;

  /**
   * @brief Cube texture that can be rendered to.
   */
  const SharedTextureCube texture_cube;

  int width() const;
  int height() const;

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
