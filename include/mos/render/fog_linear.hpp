#ifndef FOG_LINEAR_HPP
#define FOG_LINEAR_HPP

#include <glm/glm.hpp>

namespace mos {
class FogLinear {
public:
  FogLinear(const glm::vec3 &color = glm::vec3(1.0f),
            const float near = 2000.0f, const float far = 3000.0f);
  glm::vec3 color;
  float near;
  float far;
  ~FogLinear();
};
}

#endif // FOG_LINEAR_HPP
