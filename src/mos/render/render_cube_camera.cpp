#include <mos/render/render_cube_camera.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace mos {

RenderCubeCamera::RenderCubeCamera(const glm::vec3 &position,
                                   const glm::vec2 &resolution) : position_(position),
                                                                  resolution(resolution),
                                                                  projection(glm::perspective(glm::pi<float>() / 2.0f,
                                                                                              aspect_ratio(),
                                                                                              0.1f,
                                                                                              100.0f)) {
  update_views();
}
glm::vec3 RenderCubeCamera::position() const {
  return position_;
}
void RenderCubeCamera::position(const glm::vec3 &position) {
  position_;
  update_views();
}
float RenderCubeCamera::aspect_ratio() const {
  return resolution.x / resolution.y;
}
void RenderCubeCamera::update_views() {
  views[0] = glm::lookAt(position_, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  views[1] = glm::lookAt(position_, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  views[2] = glm::lookAt(position_, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  views[3] = glm::lookAt(position_, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  views[4] = glm::lookAt(position_, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  views[5] = glm::lookAt(position_, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

}
}