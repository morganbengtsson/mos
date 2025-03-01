#pragma once

#include <array>
#include <glm/glm.hpp>
#include <mos/gfx/camera.hpp>

namespace mos::gfx {

/** Camera for environment rendering. */
class Cube_camera final {
public:
  explicit Cube_camera(const glm::vec3 &position = glm::vec3(0.0F, 0.0F, 1.25F),
                       float near = 0.1F, float far = 100.0F);

  [[nodiscard]] auto position() const -> glm::vec3;

  /** Set position. */
  auto position(const glm::vec3 &position) -> void;

  /** Set near and far planes. */
  auto near_far(float near, float far) -> void;

  /** Cameras for eac axis. */
  std::array<Camera, 6> cameras;
};
} // namespace mos::gfx
