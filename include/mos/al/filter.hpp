#pragma once

#include <chrono>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

namespace mos::al {

class Filter {
  friend class Renderer;

public:
  Filter(const Filter &filter) = delete;
  Filter(Filter &&filter) noexcept;

  Filter &operator=(const Filter &filter) = delete;
  Filter &operator=(Filter &&filter) noexcept;
  ~Filter();

  ALuint id{0};
private:
  Filter();
  void release();
};
}
