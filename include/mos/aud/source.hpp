#pragma once

#include <atomic>
#include <glm/vec3.hpp>

namespace mos::aud {

/** Base class for the audio source. Either streamed or direct from memory. */
class Source final {
public:
  Source(const glm::vec3 &position = glm::vec3(0.0F),
         const glm::vec3 &velocity = glm::vec3(0.0F), float pitch = 1.0F,
         float gain = 1.0F, bool loop = false, bool playing = false,
         float obstructed = 0.0F);

  /** Unique id. */
  [[nodiscard]] auto id() const -> unsigned int;

  /** Position. */
  glm::vec3 position{glm::vec3(0.0F)};

  /** Velocity */
  glm::vec3 velocity{glm::vec3(0.0F)};

  /** Pitch. */
  float pitch{1.0F};

  /** Volume */
  float gain{1.0F};

  /** The source is looping */
  bool loop{false};

  /** The source is playing. */
  bool playing{false};

  /** How much the source is obstructed. 0.0 - 1.0 */
  float obstructed{0.0F};

private:
  static std::atomic_uint current_id_;
  unsigned int id_;
};
} // namespace mos::aud
