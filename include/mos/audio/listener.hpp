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
  Listener(const glm::vec3 &position = glm::vec3(0.0f),
           const glm::vec3 &orientation = glm::vec3(0.0f, 1.0f, 0.0f),
           const glm::vec3 &velocity = glm::vec3(0.0f));
  ~Listener();

  /**
   * @brief position of listener.
   */
  glm::vec3 position;

  /**
   * @brief orientation of listener.
   */
  glm::vec3 orientation;

  /**
   * @brief velocity of listener.
   */
  glm::vec3 velocity;
};
}

#endif /* MOS_LISTENER_H */
