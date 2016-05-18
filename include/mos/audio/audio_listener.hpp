#ifndef MOS_AUDIO_LISTENER_H
#define MOS_AUDIO_LISTENER_H

#include <glm/glm.hpp>

namespace mos {

/**
* @brief The AudioListener class, where the audio system listens from.
*/
class AudioListener {
public:
  /**
   * @brief Listener constructor.
   * @param position
   * @param orientation
   * @param velocity
   */
  AudioListener(const glm::vec3 &position = glm::vec3(.0f),
           const glm::vec3 &direction = glm::vec3(.0f, 1.0f, .0f),
           const glm::vec3 &up = glm::vec3(.0f, .0f, 1.0f),
           const glm::vec3 &velocity = glm::vec3(.0f),
           const float gain = 1.0f);
  ~AudioListener();

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

#endif /* MOS_AUDIO_LISTENER_H */
