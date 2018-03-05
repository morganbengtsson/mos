#include <mos/render/fog.hpp>
namespace mos {
namespace gfx {
Fog::Fog(const glm::vec3 &color,
         const float attenuation_factor) : Fog(color,
                                               color,
                                               attenuation_factor) {}

Fog::Fog(const glm::vec3 &color_near,
         const glm::vec3 &color_far,
         const float attenuation_factor) : color_near(color_near), color_far(color_far),
                                           attenuation_factor(attenuation_factor) {}

Fog::~Fog() {}
}
}
