#pragma once
#include <atomic>
#include <glm/vec3.hpp>

namespace mos {
namespace aud {

/** Base class for the audio source. Either streamed or direct from memory. */
class Source final {
public:
  Source(const glm::vec3 &position = glm::vec3(0.0f),
         const glm::vec3 &velocity = glm::vec3(0.0f),
         float pitch = 1.0f,
         float gain = 1.0f,
         bool loop = false,
         bool playing = false,
         float obstructed = 0.0f);

  /** Unique id. */
  unsigned int id() const;

  glm::vec3 position;

  /** Velocity */
  glm::vec3 velocity;
  /** Pitch. */
  float pitch;
  /** Volume */
  float gain;
  /** The source is looping */
  bool loop;
  /** The source is playing. */
  bool playing;
  /** How much the source is obstructed. 0.0 - 1.0 */
  float obstructed;

private:
  static std::atomic_uint current_id_;
  unsigned int id_;
};
}
}
