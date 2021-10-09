#pragma once

#include <chrono>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

#include <mos/al/resource.hpp>
#include <mos/apu/source.hpp>

namespace mos::aud {
class Source;
}

namespace mos::al {

class Source;

/** Filter living on the APU */
class Filter final : public Resource {
  friend class Renderer;

public:
  void update(const apu::Source &source, const float dt);

private:
  Filter(const Source &source);
};
} // namespace mos::al
