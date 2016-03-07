#ifndef MOS_CAMERA_H
#define MOS_CAMERA_H

#include <glm/glm.hpp>

namespace mos {

/**
 * @brief The Camera class
 */
class Camera {
public:
  /**
   * @brief Camera
   * @param eye Where the camera is located.
   * @param center Where the camera is centered.
   * @param projection Projection matrix (ortho/perspective).
   * @param up Up direction.
   */
  Camera(const glm::vec3 &position, const glm::vec3 &center,
         const glm::mat4 &projection,
         const glm::vec3 &up = glm::vec3(0.0f, 0.0f, 1.0f));

  /**
   * @brief up
   * @return up vector of camera.
   */
  glm::vec3 up() const;

  /**
   * @brief Set up vector.
   * @param up
   */
  void up(const glm::vec3 &up);

  /**
   * @brief position
   * @return Position of camera.
   */
  glm::vec3 position() const;

  /**
   * @brief Set position.
   * @param position
   */
  void position(const glm::vec3 &position);

  /**
   * @brief Get center point.
   * @return
   */
  glm::vec3 center() const;

  /**
   * @brief Get center point.
   * @param center
   */
  void center(const glm::vec3 &center);

  /**
   * @brief direction
   * @return
   */
  glm::vec3 direction() const;

  /**
   * @brief Projection matrix.
   */
  glm::mat4 projection;

  /**
   * @brief View matrix.
   */
  glm::mat4 view;

private:
  void update_view();
  glm::vec3 up_;
  glm::vec3 center_;
  glm::vec3 position_;
};
}

#endif // MOS_CAMERA_H
