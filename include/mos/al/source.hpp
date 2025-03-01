#pragma once

#include <chrono>

#include <mos/al/resource.hpp>
#include <mos/apu/source.hpp>

namespace mos::al {

/** Source living on the APU */
class Source final : public Resource {
  friend class Renderer;

public:
  using Time_point = std::chrono::time_point<std::chrono::system_clock,
                                             std::chrono::nanoseconds>;

  void update(const mos::apu::Source &Source);
  void stop();

private:
  Source(const mos::apu::Source &Source);
};
} // namespace mos::al
