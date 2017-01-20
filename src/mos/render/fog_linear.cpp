#include <mos/render/fog_linear.hpp>

namespace mos {
FogLinear::FogLinear(const glm::vec3 &color,
                     const float near,
                     const float far,
                     const float linear_attenuation_factor,
                     const float exponential_attenuation_factor,
                     const float exponential_attenuation_squared_factor)
    : color(color), near(near), far(far), linear_attenuation_factor(linear_attenuation_factor),
      exponential_attenuation_factor(exponential_attenuation_factor),
      exponential_attenuation_squared_factor(exponential_attenuation_squared_factor) {}

FogLinear::~FogLinear() {}
}
