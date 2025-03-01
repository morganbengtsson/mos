#pragma once

#include <glm/glm.hpp>

namespace mos::aud {

/** Where the audio system listens from. */
class Listener final {
public:
  Listener(const glm::vec3 &position = glm::vec3(.0f),
           const glm::vec3 &direction = glm::vec3(.0F, 1.0F, .0F),
           const glm::vec3 &up_direction = glm::vec3(.0f, .0F, 1.0F),
           const glm::vec3 &velocity = glm::vec3(.0F), float gain = 1.0F);

  /** Listener position. */
  glm::vec3 position{0.0F};

  /** Listener direction. */
  glm::vec3 direction{glm::vec3(0.0F, 1.0F, 0.0F)};

  /** World upwards direction. */
  glm::vec3 up{glm::vec3(0.0F, 0.0F, 1.0F)};

  /** Listener velocity. */
  glm::vec3 velocity{0.0F};

  /** Global audio gain. */
  float gain{1.0F};
};
} // namespace mos::aud
