#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/camera.hpp>
#include <mos/gfx/target.hpp>

namespace mos {
namespace gfx {

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
                 const glm::vec3 &color = glm::vec3(0.0f),
                 const float strength = 100.0f);
  virtual ~Light();

  /**
   * @brief Set spot angle.
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
  glm::vec3 color;

  /**
   * @brief strength of the lamp in watts.
   */
  float strength;

  /**
   * @brief Camera to render shadow map from.
   */
  Camera camera;

  /**
  * @brief Shadowmap if used.
  */
  SharedTexture2D shadow_map;

  /**
   * Render target for the
   */
  Target target;
private:
  /**
  * @brief angle.
  */
  float angle_;
};
}
}

