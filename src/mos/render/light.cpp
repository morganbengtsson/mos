#include <mos/render/light.hpp>

namespace mos {

Light::Light(const glm::vec3 &position, const glm::vec3 &diffuse,
             const glm::vec3 &specular, const glm::vec3 &ambient)
    : position(position), diffuse(diffuse), specular(specular),
      ambient(ambient) {}

Light::~Light() {}
}
