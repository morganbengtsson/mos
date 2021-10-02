#pragma once

#include <mos/aud/buffer.hpp>

namespace mos::al {
  class Renderer;
}


namespace mos::apu {

class Buffer {
  friend class mos::al::Renderer;
  friend class Sound;

  /** Unique id. */
  auto id() const -> unsigned int;

  /** Get number of channels. */
  auto channels() const -> int;

  /** Get sample rate */
  auto sample_rate() const -> int;

  /** Duration in seconds. */
  auto duration() const -> float;

  /** Size of samples container. */
  auto size() const -> size_t;

private:
  explicit Buffer(const aud::Buffer &buffer);
  explicit Buffer();

  unsigned int id_ = -1;
  int channels_ = 0;
  int sample_rate_ = 0;
  size_t size_ = 0;
};

}
