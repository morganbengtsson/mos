#include <mos/render/light.hpp>

namespace mos {

Light::Light(const glm::vec3 &position,
             const glm::vec3 &diffuse,
             const glm::vec3 &specular,
             const glm::vec3 &ambient,
             const float linear_attenuation_factor,
             const float quadratic_attenuation_factor)
    : position(position), diffuse(diffuse), specular(specular),
      ambient(ambient), linear_attenuation_factor(linear_attenuation_factor),
      quadratic_attenuation_factor(quadratic_attenuation_factor) {}

Light::~Light() {}
}
