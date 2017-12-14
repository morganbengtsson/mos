#ifndef FOG_LINEAR_HPP
#define FOG_LINEAR_HPP

#include <glm/glm.hpp>

namespace mos {
/**
 * @brief Class that represents linear and exponential fog.
 */
class Fog {
public:
  /**
   * @param color is the color of the fog.
   * @param near is distance of near plane.
   * @param far is distance of far plane.
   * @param linear_factor represents how much linear fog.
   * @param exponential_factor represents how much exponential fog.
   * @param exponential_attenuation_factor represents how dense exponential fog.
   * @param exponential_power is the power of the exponential fog function.
   */
  Fog(const glm::vec3 &color,
            const float attenuation_factor = 0.0f);

  /**
   *
   * @param color_near is the color of the near plane.
   * @param color_far is the color of the far plane.
   * @param near is distance of near plane.
   * @param far is distance of far plane.
   * @param linear_factor represents how much linear fog.
   * @param exponential_factor represents how much exponential fog.
   * @param exponential_attenuation_factor represents how dense exponential fog.
   * @param exponential_power is the power of the exponential fog function.
   */
  Fog(const glm::vec3 &color_near = glm::vec3(1.0f),
      const glm::vec3 &color_far = glm::vec3(1.0f),
      const float attenuation_factor = 0.0f);

  ~Fog();

  glm::vec3 color_near;
  glm::vec3 color_far;
  float attenuation_factor;

};
}

#endif // FOG_LINEAR_HPP
