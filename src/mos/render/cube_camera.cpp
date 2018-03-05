#include <mos/render/cube_camera.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace mos {
namespace gfx {
RenderCubeCamera::RenderCubeCamera(const glm::vec3 &position,
                                   const float aspect_ratio) : projection(glm::perspective(glm::half_pi<float>(),
                                                                                           aspect_ratio,
                                                                                           0.1f,
                                                                                           100.0f)),
                                                               up(glm::vec3(0.0f, 0.0f, 1.0f)),
                                                               cameras{{RenderCamera(position,
                                                                                          position + glm::vec3(1.0f,
                                                                                                               0.0f,
                                                                                                               0.0f),
                                                                                          projection,
                                                                                          glm::vec3(0.0f, -1.0f, 0.0f)),
                                                                        RenderCamera(position,
                                                                                          position + glm::vec3(-1.0f,
                                                                                                               0.0f,
                                                                                                               0.0f),
                                                                                          projection,
                                                                                          glm::vec3(0.0f, -1.0f, 0.0f)),
                                                                        RenderCamera(position,
                                                                                          position + glm::vec3(0.0f,
                                                                                                               1.0f,
                                                                                                               0.0f),
                                                                                          projection,
                                                                                          glm::vec3(0.0f, 0.0f, 1.0f)),
                                                                        RenderCamera(position,
                                                                                          position + glm::vec3(0.0f,
                                                                                                               -1.0f,
                                                                                                               0.0f),
                                                                                          projection,
                                                                                          glm::vec3(0.0f, 0.0f, -1.0f)),
                                                                        RenderCamera(position,
                                                                                          position + glm::vec3(0.0f,
                                                                                                               0.0f,
                                                                                                               1.0f),
                                                                                          projection,
                                                                                          glm::vec3(0.0f, -1.0f, 0.0f)),
                                                                        RenderCamera(position,
                                                                                          position + glm::vec3(0.0f,
                                                                                                               0.0f,
                                                                                                               -1.0f),
                                                                                          projection,
                                                                                          glm::vec3(0.0f,
                                                                                                    -1.0f,
                                                                                                    0.0f))}} {}
glm::vec3 RenderCubeCamera::position() const {
  return cameras[0].position();
}
RenderCubeCamera::RenderCubeCamera(const RenderCubeCamera &camera)
    : projection(camera.projection), up(camera.up), cameras(camera.cameras) {
}

RenderCubeCamera &RenderCubeCamera::operator=(const RenderCubeCamera &other) {
  //TODO: Implement swap
  projection = other.projection;
  up = other.up;
  cameras = other.cameras;
  return *this;
}
}
}