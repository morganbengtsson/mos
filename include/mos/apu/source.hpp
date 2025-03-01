#pragma once

#include <glm/vec3.hpp>
#include <mos/aud/source.hpp>

namespace mos::al {
class Renderer;
} // namespace mos::al

namespace mos::apu {

/** Source living on the APU */
class Source final {
  friend class mos::al::Renderer;
  friend class Sound;
  friend class Sound_stream;
private:
  explicit Source(const aud::Source &source);

public:

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
  unsigned int id_;
};
} // namespace mos::apu
