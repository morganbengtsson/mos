#include <mos/render/light.hpp>
#include <glm/gtx/transform.hpp>

namespace mos {

Light::Light(const glm::vec3 &position,
             const glm::vec3 &center,
             const float angle,
             const glm::vec3 &diffuse,
             const glm::vec3 &specular,
             const float linear_attenuation_factor,
             const float quadratic_attenuation_factor)
    : diffuse(diffuse), specular(specular), angle_(angle),
      linear_attenuation_factor(linear_attenuation_factor),
      quadratic_attenuation_factor(quadratic_attenuation_factor),
      camera(position, center, glm::perspective(angle, 1.0f, 1.0f, 100.0f),
                        glm::vec3(0.0f, 0.0001f, 1.0f)),
      shadow_map(mos::SharedTexture2D(new mos::Texture2D(
          512, 512, mos::Texture::Format::RG32F,
          mos::Texture::Wrap::CLAMP_TO_BORDER, true))){}

Light::~Light() {}

void Light::position(const glm::vec3 &position) {
  camera.position(position);
}

glm::vec3 Light::position() const{
  return camera.position();
}

void Light::angle(const float angle) {
  angle_ = angle;
  camera.projection = glm::perspective(angle_, 1.0f, 1.0f, 100.0f);
}

float Light::angle() const {
  return angle_;
}

void Light::center(const glm::vec3 &center) {
  camera.center(center);
}

glm::vec3 Light::center() const {
  return camera.center();
}

glm::vec3 Light::direction() const {
  return camera.direction();
}
}
