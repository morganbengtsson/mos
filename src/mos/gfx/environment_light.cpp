#include <mos/gfx/environment_light.hpp>
namespace mos {
namespace gfx {
EnvironmentLight::EnvironmentLight(const glm::vec3 &position,
                                   const glm::vec3 &extent,
                                   const float strength,
                                   const glm::uvec2 resolution)
    : texture_(resolution.x, resolution.y),
      box_(Box(position, extent)),
      strength(strength),
      cube_camera_(position) {
}
EnvironmentLight::EnvironmentLight(const EnvironmentLight &light) :
    texture_(light.texture_),
    box_(light.box_),
    strength(light.strength),
    cube_camera_(light.cube_camera_),
    target_(light.target_) {}
EnvironmentLight &EnvironmentLight::operator=(const EnvironmentLight &other) {
  texture_ = other.texture_;
  box_ = other.box_;
  strength = other.strength;
  cube_camera_ = other.cube_camera_;
  target_ = other.target_;
  return *this;
}
void EnvironmentLight::position(const glm::vec3 &position) {
  box_.position = position;
  cube_camera_.position(position);
}
glm::vec3 EnvironmentLight::position() const {
  return box_.position;
}
void EnvironmentLight::extent(const glm::vec3 &extent) {
  box_.extent = extent;
}
glm::vec3 EnvironmentLight::extent() const {
  return box_.extent;
}
}
}