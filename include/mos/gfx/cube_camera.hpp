#pragma once

#include <glm/glm.hpp>
#include <array>
#include <mos/gfx/camera.hpp>

namespace mos {
namespace gfx {
class CubeCamera {
private:
  void update_views();
  glm::mat4 projection;
  glm::vec3 up;
public:
  CubeCamera(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 1.25f),
                   const float aspect_ratio = 1.0f);
  CubeCamera(const CubeCamera &camera);
  CubeCamera &operator=(const CubeCamera &other);

  glm::vec3 position() const;
  std::array<Camera, 6> cameras;
};
}
}