#pragma once

#include <chrono>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

#include <mos/al/resource.hpp>

namespace mos::aud {
class Source;
}

namespace mos::al {

class Source;

class Filter final : public Resource {
  friend class Renderer;

public:
  void update(const aud::Source &source, const float dt);

private:
  Filter(const Source &source);
};
}
