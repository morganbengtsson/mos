#ifndef FOG_HPP
#define FOG_HPP

#include <glm/glm.hpp>

namespace mos {
class FogExp {
public:
  FogExp();
  FogExp(const glm::vec3 &color, float density);
  FogExp(const glm::vec4 & fog);
  glm::vec3 color;
  float density;
  ~FogExp();
};
}

#endif // FOG_HPP
