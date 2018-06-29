#include <mos/gfx/light.hpp>
#include <glm/gtx/transform.hpp>
namespace mos {
namespace gfx {
Light::Light(const glm::vec3 &position,
             const glm::vec3 &center,
             const float angle,
             const glm::vec3 &color,
             const float strength,
             const float near,
             const float far)
    : color(color), angle_(angle),
      camera(position, center, glm::perspective(angle, 1.0f, near, far),
             glm::vec3(0.0f, 0.0001f, 1.0f)), strength(strength), near_(near), far_(far) {}

Light::~Light() {}

void Light::position(const glm::vec3 &position) {
  camera.position(position);
}

glm::vec3 Light::position() const {
  return camera.position();
}

void Light::angle(const float angle) {
  angle_ = angle;
  camera.projection = glm::perspective(angle_, 1.0f, near_, far_);
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
}