#pragma once
#include <atomic>
#include <glm/vec3.hpp>

namespace mos {
namespace aud {

/** Base class for the audio sources. Either streamed or direct from memory. */
class Source final {
public:
  Source(const glm::vec3 &position = glm::vec3(0.0f),
         const glm::vec3 &velocity = glm::vec3(0.0f),
         const float pitch = 1.0f,
         const float gain = 1.0f,
         const bool loop = false,
         const bool playing = false,
         const float obstructed = 0.0f);
  ~Source();

  /** Unique id. */
  unsigned int id() const;

  glm::vec3 position;
  glm::vec3 velocity;
  float pitch;
  float gain;
  bool loop;
  bool playing;
  float obstructed;

private:
  static std::atomic_uint current_id_;
  unsigned int id_;
};
}
}
