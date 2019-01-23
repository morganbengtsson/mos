#pragma once

#include <array>
#include <glm/glm.hpp>
#include <mos/gfx/camera.hpp>

namespace mos {
namespace gfx {

/** Camera for environment rendering. */
class Cube_camera final {
public:
  explicit Cube_camera(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 1.25f),
             float near = 0.1f,
             float far = 100.0f);

  glm::vec3 position() const;

  /** Set position. */
  void position(const glm::vec3 &position);

  /** Set near and far planes. */
  void near_far(float near, float far);

  /** Cameras for eac axis. */
  std::array<Camera, 6> cameras;
};
}
}
