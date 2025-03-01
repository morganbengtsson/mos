#pragma once

#include <mos/al/resource.hpp>
#include <mos/apu/source.hpp>

namespace mos::aud {
class Source;
} // namespace mos::aud

namespace mos::al {

class Source;

/** Filter living on the APU */
class Filter final : public Resource {
  friend class Renderer;

public:
  void update(const apu::Source &source, float delta_time);

private:
  Filter(const Source &source);
};
} // namespace mos::al
