#include <mos/graphics/fog_exp.hpp>

namespace mos {
  FogExp::FogExp(){}
  FogExp::FogExp(const glm::vec3 &color, float density)
    : color(color), density(density) {}
FogExp::FogExp(const glm::vec4 &fog) : color(fog.x, fog.y, fog.z), density(fog.w) {}
FogExp::~FogExp() {}
}
