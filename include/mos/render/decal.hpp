#pragma once

#include <glm/glm.hpp>
#include <mos/render/material.hpp>

namespace mos {
namespace gfx {
/**
 * @brief Decal projection.
 */
class Decal {
public:
  Decal();
  Decal(const glm::mat4 &projection,
        const glm::mat4 &view,
        const Material &material);
  glm::mat4 projection;
  glm::mat4 view;
  Material material;
};
}
}
