#pragma once

#include <chrono>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

#include <mos/aud/source.hpp>

namespace mos::al {

class Source {
  friend class Renderer;
public:
  using Time_point = std::chrono::time_point<std::chrono::system_clock,
                                             std::chrono::nanoseconds>;

public:
  Source(const Source &Source) = delete;
  Source(Source &&Source) noexcept;

  Source &operator=(const Source &Source) = delete;
  Source &operator=(Source &&Source) noexcept;
  ~Source();

  void update(const mos::aud::Source &Source);
  void stop();

  ALuint id{0};
  Time_point modified; // TODO:

private:
  Source(const mos::aud::Source &Source);
  void release();
};
}
