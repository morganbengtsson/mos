#pragma once

#include <array>
#include <glm/glm.hpp>
#include <mos/gfx/camera.hpp>

namespace mos {
namespace gfx {

/** Camera for environment rendering. */
class CubeCamera {
public:
  CubeCamera(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 1.25f),
             const float near = 0.1,
             const float far = 100.0f);

  glm::vec3 position() const;
  void position(const glm::vec3 &position);
  void near_far(const float near, const float far);
  std::array<Camera, 6> cameras;
};
}
}