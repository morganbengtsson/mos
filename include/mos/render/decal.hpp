#ifndef MOS_DECAL_HPP
#define MOS_DECAL_HPP

#include <glm/glm.hpp>
#include <mos/render/texture.hpp>

namespace mos {
class Decal {
public:
  glm::mat4 transform;
  glm::mat4 projection;
  glm::mat4 view;
  mos::SharedTexture texture;
};
}

#endif //MOS_DECAL_HPP
