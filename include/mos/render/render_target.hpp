#ifndef MOS_RENDER_TARGET_HPP
#define MOS_RENDER_TARGET_HPP

#include <memory>
#include <atomic>
#include <optional.hpp>
#include <glm/glm.hpp>
#include <mos/render/texture.hpp>

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
   * @brief RenderTarget
   * @param width
   * @param height
   */
  explicit RenderTarget(const unsigned int width = 256,
                        const unsigned int height = 256);

  /**
   * @brief Target
   * @param resolution
   */
  explicit RenderTarget(const glm::ivec2 &resolution = glm::ivec2(256, 256));

  /**
   * @brief The texture that is rendered to.
   * @todo Should probably not be shared.
   */
  std::shared_ptr<mos::Texture> texture;

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
