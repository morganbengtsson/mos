#include <mos/render/environment_light.hpp>

namespace mos {

EnvironmentLight::EnvironmentLight(const SharedTextureCube &texture, const RenderBox &box) : texture(texture), box(box) {
}

}