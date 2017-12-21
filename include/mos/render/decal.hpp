#ifndef MOS_DECAL_HPP
#define MOS_DECAL_HPP

#include <glm/glm.hpp>
#include <mos/render/material.hpp>

namespace mos {
class Decal {
public:
  Decal();
  Decal(const glm::mat4 &projection,
        const glm::mat4 &view,
        const mos::Material &material);
  glm::mat4 projection;
  glm::mat4 view;
  mos::Material material;
};
}

#endif //MOS_DECAL_HPP
