#pragma once

#include <chrono>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

#include <mos/aud/buffer.hpp>

namespace mos::al {

class Buffer {
  friend class Renderer;
public:
  using Time_point = std::chrono::time_point<std::chrono::system_clock,
                                             std::chrono::nanoseconds>;

public:
  Buffer(const Buffer &buffer) = delete;
  Buffer(Buffer &&buffer) noexcept : id(buffer.id) {
    buffer.id = 0;
  }

  Buffer &operator=(const Buffer &buffer) = delete;
  Buffer &operator=(Buffer &&buffer) noexcept {
    if (this != &buffer) {
      release();
      std::swap(id, buffer.id);
    }
    return *this;
  }
  ~Buffer() {
    release();
  }

  ALuint id{0};
  Time_point modified; // TODO:

private:
  Buffer(const mos::aud::Buffer &buffer) {
    alGenBuffers(1, &id);
    auto format =
        buffer.channels() == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    long data_size = std::distance(buffer.begin(), buffer.end());
    const ALvoid *data = buffer.data();
    alBufferData(id, format, data, data_size * sizeof(short),
                 buffer.sample_rate());
  }
  void release() {
    alDeleteBuffers(1, &id);
    id = 0;
  }
};
}
