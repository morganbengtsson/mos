#ifndef MOS_RENDERCUBECAMERA_HPP
#define MOS_RENDERCUBECAMERA_HPP

#include <glm/glm.hpp>
#include <array>
#include <mos/render/render_camera.hpp>

namespace mos {
class RenderCubeCamera {
private:
  void update_views();
  glm::mat4 projection;
  glm::vec3 up;
public:
  RenderCubeCamera(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 1.25f),
                   const float aspect_ratio = 1.0f);
  RenderCubeCamera(const RenderCubeCamera &camera);
  RenderCubeCamera& operator=(const RenderCubeCamera &other);

  glm::vec3 position() const;
  std::array<mos::RenderCamera, 6> cameras;
};
}

#endif //MOS_RENDERCUBECAMERA_HPP
