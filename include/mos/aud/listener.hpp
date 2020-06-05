#pragma once

#include <glm/glm.hpp>

namespace mos::aud {

/** Where the audio system listens from. */
class Listener final {
public:
  Listener(const glm::vec3 &position = glm::vec3(.0f),
           const glm::vec3 &direction = glm::vec3(.0f, 1.0f, .0f),
           const glm::vec3 &up = glm::vec3(.0f, .0f, 1.0f),
           const glm::vec3 &velocity = glm::vec3(.0f), float gain = 1.0f);

  /** Listener position. */
  glm::vec3 position{0.0f};

  /** Listener direction. */
  glm::vec3 direction{glm::vec3(0.0f, 1.0f, 0.0f)};

  /** World upwards direction. */
  glm::vec3 up{glm::vec3(0.0f, 0.0f, 1.0f)};

  /** Listener velocity. */
  glm::vec3 velocity{0.0f};

  /** Global audio gain. */
  float gain{1.0f};
};
}

