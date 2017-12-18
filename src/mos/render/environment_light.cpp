#include <mos/render/environment_light.hpp>

namespace mos {

EnvironmentLight::EnvironmentLight(const SharedTextureCube &texture,
                                   const RenderBox &box,
                                   const float strength)
    : texture(texture), box(box), strength(strength) {
}

}