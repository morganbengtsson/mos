#pragma once
#include <glm/glm.hpp>

namespace mos::gfx {

/** Fog with exponential falloff and near/far color blending. */
class Fog final {
public:
  /** @param color Color of the fog.
   * @param attenuation_factor Fog density. */
  explicit Fog(const glm::vec3 &color, float attenuation_factor = 0.0F,
               float min = 0.0F, float max = 1.0F);

  /** @param color_near Close fog color.
   * @param color_far Far fog color.
   * @param attenuation_factor Fog density. */
  explicit Fog(const glm::vec3 &color_near = glm::vec3(1.0F),
               const glm::vec3 &color_far = glm::vec3(1.0F),
               float attenuation_factor = 0.0F, float min = 0.0F,
               float max = 1.0F);

  /** Color close to the camera.*/
  glm::vec3 color_near{1.0F};

  /** Color far from the camera.*/
  glm::vec3 color_far{1.0F};

  /** Dencity of the fog. */
  float attenuation_factor{0.0F};

  /** Minimum clamp value. */
  float min{0.0F};

  /** Maximum clamp value. */
  float max{1.0F};
};
} // namespace mos::gfx
