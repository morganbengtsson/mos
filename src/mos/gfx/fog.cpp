#include <mos/gfx/fog.hpp>
namespace mos::gfx {
Fog::Fog(const glm::vec3 &color, const float attenuation_factor,
         const float min, const float max)
    : Fog(color, color, attenuation_factor, min, max) {}

Fog::Fog(const glm::vec3 &color_near, const glm::vec3 &color_far,
         const float attenuation_factor, const float min, const float max)
    : color_near(color_near), color_far(color_far),
      attenuation_factor(attenuation_factor), min(min), max(max) {}
}
