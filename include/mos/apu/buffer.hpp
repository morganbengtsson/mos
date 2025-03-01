#pragma once

#include <cstddef>
#include <mos/aud/buffer.hpp>

namespace mos::al {
class Renderer;
} // namespace mos::al

namespace mos::apu {

class Buffer {
  friend class mos::al::Renderer;
  friend class Sound;

  /** Unique id. */
  [[nodiscard]] auto id() const -> unsigned int;

  /** Get number of channels. */
  [[nodiscard]] auto channels() const -> int;

  /** Get sample rate */
  [[nodiscard]] auto sample_rate() const -> int;

  /** Duration in seconds. */
  [[nodiscard]] auto duration() const -> float;

  /** Size of samples container. */
  [[nodiscard]] auto size() const -> std::size_t;

private:
  explicit Buffer(const aud::Buffer &buffer);
  explicit Buffer();

  unsigned int id_ = -1;
  int channels_ = 0;
  int sample_rate_ = 0;
  std::size_t size_ = 0;
};

} // namespace mos::apu
