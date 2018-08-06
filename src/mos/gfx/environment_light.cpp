#include <mos/gfx/environment_light.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace mos {
namespace gfx {

EnvironmentLight::EnvironmentLight(const glm::vec3 &position,
                                   const glm::vec3 &extent,
                                   const float strength)
    :
      box_(glm::translate(glm::mat4(1.0f), position), extent),
      strength(strength),
      cube_camera_(position, 1.0f, 0.01f, glm::length(extent)) {
}

void EnvironmentLight::position(const glm::vec3 &position) {
  box_.transform[3] = glm::vec4(position, 0.0f);
  cube_camera_.position(position);
}
glm::vec3 EnvironmentLight::position() const {
  return glm::vec3(box_.transform[3]);
}
void EnvironmentLight::extent(const glm::vec3 &extent) {
  box_.extent = extent;
}
glm::vec3 EnvironmentLight::extent() const {
  return box_.extent;
}
bool EnvironmentLight::inside(const glm::vec3 &point) const {
  return box_.inside(point);
}
}
}