#include <mos/gfx/cube_camera.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace mos {
namespace gfx {
CubeCamera::CubeCamera(const glm::vec3 &position,
                       const float aspect_ratio) : projection_(glm::perspective(glm::half_pi<float>(),
                                                                               aspect_ratio,
                                                                               0.1f,
                                                                               100.0f)),
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
CubeCamera::CubeCamera(const CubeCamera &camera)
    : projection_(camera.projection_), up_(camera.up_), cameras(camera.cameras) {
}

CubeCamera &CubeCamera::operator=(const CubeCamera &other) {
  //TODO: Implement swap
  projection_ = other.projection_;
  up_ = other.up_;
  cameras = other.cameras;
  return *this;
}
}
}