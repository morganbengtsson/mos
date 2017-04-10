#ifndef MOS_LIGHT_H
#define MOS_LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <mos/render/texture_2d.hpp>
#include <mos/render/render_camera.hpp>

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
  explicit Light(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 2.0f),
                 const glm::vec3 &center = glm::vec3(0.0f, 0.0f, 0.0f),
                 const float angle = glm::half_pi<float>(),
                 const glm::vec3 &diffuse = glm::vec3(0.0f),
                 const glm::vec3 &specular = glm::vec3(0.0f),
                 const float linear_attenuation_factor = 0.0f,
                 const float quadratic_attenuation_factor = 0.0f);
  virtual ~Light();

  /**
   * @brief Set spot anagle.
   * @param angle of the spotlight.
   */
  void angle(const float angle);

   /**
   * @brief get spot angle.
   * @return angle of the spotlight.
   */
  float angle() const;

  /**
   * @breif Set position.
   * @param position
   */
  void position(const glm::vec3 &position);

  /**
   * @brief Get position.
   * @return
   */
  glm::vec3 position() const;

  /**
   * @brief Set center/focus of spotlight
   * @param center of the spotlight.
   */
  void center(const glm::vec3 &center);

  /**
   *@brief Get center of spotlight.
   * @return center of the spotlight.
   */
  glm::vec3 center() const;

  /**
   * @brief Get the direction of spotlight.
   * @return direction.
   */
  glm::vec3 direction() const;

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
   * @brief Camera to render shadow map from.
   */
  RenderCamera camera;

  /**
  * @brief Shadowmap if used.
  */
  SharedTexture2D shadow_map;
private:
  /**
  * @brief angle.
  */
  float angle_;
};
}

#endif /* MOS_LIGHT_H */
