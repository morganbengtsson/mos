#ifndef MOS_RENDERCUBECAMERA_HPP
#define MOS_RENDERCUBECAMERA_HPP

#include <glm/glm.hpp>
#include <array>
#include <mos/render/render_camera.hpp>

namespace mos {
class RenderCubeCamera {
private:
  void update_views();
  const glm::mat4 projection;
  const glm::vec3 up;
public:
  RenderCubeCamera(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 1.25f),
                   const glm::vec2 &resolution = glm::vec2(128.0f, 128.0f));

  std::array<mos::RenderCamera, 6> cameras;



};
}

#endif //MOS_RENDERCUBECAMERA_HPP
