#include <mos/gfx/environment_light.hpp>
namespace mos {
namespace gfx {
EnvironmentLight::EnvironmentLight(const glm::vec3 &position,
                                   const glm::vec3 &extent,
                                   const float strength,
                                   const glm::uvec2 resolution)
    : texture(resolution.x, resolution.y),
      box(Box(position, extent)),
      strength(strength),
      cube_camera(position) {
}
EnvironmentLight::EnvironmentLight(const EnvironmentLight &light) :
    texture(light.texture),
    box(light.box),
    strength(light.strength),
    cube_camera(light.cube_camera),
    target(light.target) {}
EnvironmentLight &EnvironmentLight::operator=(const EnvironmentLight &other) {
  texture = other.texture;
  box = other.box;
  strength = other.strength;
  cube_camera = other.cube_camera;
  target = other.target;
  return *this;
}
}
}