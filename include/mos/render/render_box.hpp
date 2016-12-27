#ifndef MOS_RENDER_BOX_HPP
#define MOS_RENDER_BOX_HPP

#include <glm/glm.hpp>

namespace mos {
class RenderBox {
public:
  RenderBox(const glm::vec3 &position, const glm::vec3 &extent);
  glm::vec3 position;
  glm::vec3 extent;
  glm::vec3 size() const;
  glm::vec3 min() const;
  glm::vec3 max() const;
};

}

#endif // MOS_RENDER_BOX_HPP
