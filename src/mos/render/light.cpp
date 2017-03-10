#include <mos/render/light.hpp>
#include <glm/gtx/transform.hpp>

namespace mos {

Light::Light(const glm::vec3 &position,
             const glm::vec3 &direction,
             const float angle,
             const glm::vec3 &diffuse,
             const glm::vec3 &specular,
             const float linear_attenuation_factor,
             const float quadratic_attenuation_factor)
    : position(position), diffuse(diffuse), specular(specular), angle(angle), direction(direction),
      linear_attenuation_factor(linear_attenuation_factor),
      quadratic_attenuation_factor(quadratic_attenuation_factor),
      camera(position, position + direction, glm::perspective(angle, 1.0f, 0.01f, 10.0f),
                        glm::vec3(0.0f, 0.0001f, 1.0f),
                        glm::vec2(512.0f, 512.0f)){}

Light::~Light() {}
}
