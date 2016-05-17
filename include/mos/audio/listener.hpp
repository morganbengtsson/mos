#ifndef MOS_LISTENER_H
#define MOS_LISTENER_H

#include <glm/glm.hpp>

namespace mos {

class Listener {
public:
  /**
   * @brief Listener constructor.
   *
   * A listener is collection of properties relative to a listener in the audio system.
   * The audio system can only handle one listener at a time. Though several can exist.
   *
   * @param position
   * @param orientation
   * @param velocity
   */
  Listener(const glm::vec3 &position = glm::vec3(.0f),
           const glm::vec3 &direction = glm::vec3(.0f, 1.0f, .0f),
           const glm::vec3 &up = glm::vec3(.0f, .0f, 1.0f),
           const glm::vec3 &velocity = glm::vec3(.0f),
           const float gain = 1.0f);
  ~Listener();

  /**
   * @brief position of listener.
   */
  glm::vec3 position;

  /**
   * @brief direction of listener.
   */
  glm::vec3 direction;

  /**
   * @brief up vector
   */
  glm::vec3 up;

  /**
   * @brief velocity of listener.
   */
  glm::vec3 velocity;

  /**
   * @brief global gian.
   */
  float gain;
};
}

#endif /* MOS_LISTENER_H */
