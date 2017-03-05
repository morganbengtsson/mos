#ifndef MOS_LIGHT_H
#define MOS_LIGHT_H

#include <glm/glm.hpp>

namespace mos {

/**
* @brief The Light class
*
* A class describing different aspects of an omnidirectional light source. Used together with the
* renderer.
*/
class Light {
public:
  /**
   * @brief Light
   * @param position
   * @param diffuse_color
   * @param specular_color
   */
  explicit Light(const glm::vec3 &position = glm::vec3(0.0f),
                 const glm::vec3 &diffuse = glm::vec3(0.0f),
                 const glm::vec3 &specular = glm::vec3(0.0f),
                 const float linear_attenuation_factor = 0.0f,
                 const float quadratic_attenuation_factor = 0.0f);
  virtual ~Light();

  /**
   * @brief position
   */
  glm::vec3 position;

  /**
   * @brief diffuse_color
   */
  glm::vec3 diffuse;

  /**
   * @brief specular_color
   */
  glm::vec3 specular;

  /**
   * @brief linear falloff
   */
  float linear_attenuation_factor;

  /**
   * @brief quadratic falloff
   */
  float quadratic_attenuation_factor;

  /**
   * @brief view
   */
  glm::mat4 view;

  /**
   * @brief projection
   */
  glm::mat4 projection;

};
}

#endif /* MOS_LIGHT_H */
