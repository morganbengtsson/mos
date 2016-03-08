#ifndef FOG_HPP
#define FOG_HPP

#include <glm/glm.hpp>

namespace mos {
class Fog {
public:
  Fog(const glm::vec3 &color, float density);
  Fog(const glm::vec4 & fog);
  glm::vec3 color;
  float density;
  ~Fog();
};
}

#endif // FOG_HPP
