#include <mos/render/fog.hpp>

namespace mos {
Fog::Fog(const glm::vec3 &color,
                     const float near,
                     const float far,
                     const float linear_factor,
                     const float exponential_factor,
                     const float exponential_attenuation_factor,
         const float exponential_power) : Fog(color, color, near, far, linear_factor, exponential_factor, exponential_attenuation_factor, exponential_power){}

Fog::~Fog() {}
Fog::Fog(const glm::vec3 &near_color,
         const glm::vec3 &far_color,
         const float near,
         const float far,
         const float linear_factor,
         const float exponential_factor,
         const float exponential_attenuation_factor,
         const float exponential_power) :color_near(near_color), color_far(far_color), near(near), far(far),
linear_factor(linear_factor),
exponential_factor(exponential_factor),
exponential_power(exponential_power),
exponential_attenuation_factor(exponential_attenuation_factor) {}
}
