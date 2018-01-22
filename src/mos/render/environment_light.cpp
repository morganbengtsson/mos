#include <mos/render/environment_light.hpp>

namespace mos {

EnvironmentLight::EnvironmentLight(const SharedTextureCube &texture,
                                   const glm::vec3 &position,
                                   const glm::vec3 &extent,
                                   const float strength)
    : texture(texture), box(RenderBox(position, extent)), strength(strength), camera(position) {
}

}