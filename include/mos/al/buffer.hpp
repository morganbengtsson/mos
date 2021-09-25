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
  Buffer(Buffer &&buffer) noexcept;

  Buffer &operator=(const Buffer &buffer) = delete;
  Buffer &operator=(Buffer &&buffer) noexcept;
  ~Buffer();

  ALuint id{0};
  Time_point modified; // TODO:

private:
  Buffer(const mos::aud::Buffer &buffer);
  void release();
};
}
