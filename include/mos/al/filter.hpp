#pragma once

#include <chrono>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

namespace mos::aud {
class Source;
}

namespace mos::al {

class Source;

class Filter {
  friend class Renderer;

public:
  Filter(const Filter &filter) = delete;
  Filter(Filter &&filter) noexcept;

  Filter &operator=(const Filter &filter) = delete;
  Filter &operator=(Filter &&filter) noexcept;
  ~Filter();

  void update(const aud::Source &source, const float dt);

  ALuint id{0};
private:
  Filter(const Source &source);
  void release();
};
}
