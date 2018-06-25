#pragma once

#include <array>
#include <glm/glm.hpp>
#include <mos/gfx/camera.hpp>

namespace mos {
namespace gfx {

/** Camera for environment rendering. */
class CubeCamera {
private:
  glm::mat4 projection_;
  glm::vec3 up_;
public:
  CubeCamera(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 1.25f),
             const float aspect_ratio = 1.0f);

  glm::vec3 position() const;
  void position(const glm::vec3 &position);
  std::array<Camera, 6> cameras;

};
}
}