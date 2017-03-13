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
    : diffuse(diffuse), specular(specular), angle_(angle),
      linear_attenuation_factor(linear_attenuation_factor),
      quadratic_attenuation_factor(quadratic_attenuation_factor),
      camera(position, position + direction, glm::perspective(angle, 1.0f, 0.01f, 10.0f),
                        glm::vec3(0.0f, 0.0001f, 1.0f)){}

Light::~Light() {}

void Light::position(const glm::vec3 &position) {
  camera.position(position);
}

glm::vec3 Light::position() const{
  return camera.position();
}

void Light::angle(const float angle) {
  angle_ = angle;
  camera.projection = glm::perspective(angle_, 1.0f, 0.01f, 10.0f);
}
float Light::angle() const {
  return angle_;
}
void Light::direction(const glm::vec3 &direction) {
  camera.direction(direction);
}
glm::vec3 Light::direction() const {
  return camera.direction();
}
void Light::center(const glm::vec3 &center) {
  camera.center(center);
}
}
