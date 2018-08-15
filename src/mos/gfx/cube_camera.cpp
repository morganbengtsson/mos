#include <mos/gfx/cube_camera.hpp>
#include <mos/gfx/cube_camera.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace mos {
namespace gfx {
CubeCamera::CubeCamera(const glm::vec3 &position, const float near, const float far)
    : projection_(glm::perspective(glm::half_pi<float>(),
                                   1.0f,
                                   near,
                                   far)),
      up_(glm::vec3(0.0f, 0.0f, 1.0f)),
      cameras{{Camera(position,
                                                                   position + glm::vec3(1.0f,
                                                                                        0.0f,
                                                                                        0.0f),
                                                                   projection_,
                                                                   glm::vec3(0.0f, -1.0f, 0.0f)),
                                                            Camera(position,
                                                                   position + glm::vec3(-1.0f,
                                                                                        0.0f,
                                                                                        0.0f),
                                                                   projection_,
                                                                   glm::vec3(0.0f, -1.0f, 0.0f)),
                                                            Camera(position,
                                                                   position + glm::vec3(0.0f,
                                                                                        1.0f,
                                                                                        0.0f),
                                                                   projection_,
                                                                   glm::vec3(0.0f, 0.0f, 1.0f)),
                                                            Camera(position,
                                                                   position + glm::vec3(0.0f,
                                                                                        -1.0f,
                                                                                        0.0f),
                                                                   projection_,
                                                                   glm::vec3(0.0f, 0.0f, -1.0f)),
                                                            Camera(position,
                                                                   position + glm::vec3(0.0f,
                                                                                        0.0f,
                                                                                        1.0f),
                                                                   projection_,
                                                                   glm::vec3(0.0f, -1.0f, 0.0f)),
                                                            Camera(position,
                                                                   position + glm::vec3(0.0f,
                                                                                        0.0f,
                                                                                        -1.0f),
                                                                   projection_,
                                                                   glm::vec3(0.0f,
                                                                             -1.0f,
                                                                             0.0f))}} {}
glm::vec3 CubeCamera::position() const {
  return cameras[0].position();
}

void CubeCamera::position(const glm::vec3 &position) {
  for (auto & camera : cameras) {
    camera.position(position);
  }
}
void CubeCamera::near_far(const float near, const float far) {
  for (auto & camera : cameras){
    camera.projection = glm::perspective(glm::half_pi<float>(),
                                         1.0f,
                                         near,
                                         far);
  }
}
}
}