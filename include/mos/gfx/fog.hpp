#pragma once
#include <glm/glm.hpp>

namespace mos {
namespace gfx {

/** Physically based fog, with exponential falloff and near/far color blending. */
class Fog final{
public:
  /** @param color Color of the fog.
   * @param attenuation_factor Fog density. */
  Fog(const glm::vec3 &color,
      const float attenuation_factor = 0.0f);

  /** @param color_near Close fog color.
   * @param color_far Far fog color.
   * @param attenuation_factor Fog density. */
  Fog(const glm::vec3 &color_near = glm::vec3(1.0f),
      const glm::vec3 &color_far = glm::vec3(1.0f),
      const float attenuation_factor = 0.0f);

  ~Fog();

  glm::vec3 color_near;
  glm::vec3 color_far;
  float attenuation_factor;
};
}
}

