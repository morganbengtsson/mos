#ifndef MOS_DECAL_HPP
#define MOS_DECAL_HPP

#include <glm/glm.hpp>
#include <mos/render/texture.hpp>

namespace mos {

class Decal {
  glm::mat4 transform;
  mos::Texture texture;
};

}

#endif //MOS_DECAL_HPP
