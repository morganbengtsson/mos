#pragma once

#include <glm/glm.hpp>

namespace mos {
namespace aud {

/** Where the audio system listens from. */
class Listener final {
public:

  Listener(const glm::vec3 &position = glm::vec3(.0f),
                const glm::vec3 &direction = glm::vec3(.0f, 1.0f, .0f),
                const glm::vec3 &up = glm::vec3(.0f, .0f, 1.0f),
                const glm::vec3 &velocity = glm::vec3(.0f),
                const float gain = 1.0f);
  ~Listener();

  glm::vec3 position;
  glm::vec3 direction;
  glm::vec3 up;
  glm::vec3 velocity;
  float gain;
};
}
}
