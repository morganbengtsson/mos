#ifndef MOS_RENDER_TARGET_CUBE_HPP
#define MOS_RENDER_TARGET_CUBE_HPP

#include <memory>
#include <atomic>
#include <optional.hpp>
#include <glm/glm.hpp>
#include <mos/render/texture_cube.hpp>

namespace mos {
  class RenderTargetCube;
  using OptTargetCube = std::experimental::optional<RenderTargetCube>;
/**
 * @brief The RenderTarget class
 *
 * Represents a rendertarget, with asociated texture. For off screen rendering.
 */
class RenderTargetCube {
public:
  /**
   * @brief RenderTarget
   * @param width
   * @param height
   */
  explicit RenderTargetCube(const float width = 128.0f,
                        const float height = 128.0f);

  /**
   * @brief Target
   * @param resolution
   */
  explicit RenderTargetCube(const glm::vec2 &resolution = glm::vec2(128, 128));

  /**
   * @brief The texture that is rendered to.
   * @todo Should probably not be shared.
   */
  std::shared_ptr<mos::TextureCube> texture;

  /**
   * @brief unique id
   * @return id
   */
  unsigned int id() const;
  int layers;

private:
  unsigned int id_;
  static std::atomic_uint current_id_;
};
}
#endif // MOS_RENDER_TARGET_HPP
