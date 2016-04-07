#include <mos/graphics/fog.hpp>

namespace mos {
  Fog::Fog(){}
  Fog::Fog(const glm::vec3 &color, float density)
    : color(color), density(density) {}
Fog::Fog(const glm::vec4 &fog) : color(fog.x, fog.y, fog.z), density(fog.w) {}
Fog::~Fog() {}
}
