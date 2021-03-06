#pragma once
#include <atomic>
#include <chrono>
#include <gli/format.hpp>
#include <gli/gli.hpp>
#include <initializer_list>
#include <memory>
#include <mos/core/tracked_container.hpp>
#include <string>
#include <vector>

namespace mos::gfx {

class Texture;

/** Texture base */
class Texture {
public:
  using TimePoint = std::chrono::time_point<std::chrono::system_clock,
                                            std::chrono::nanoseconds>;
  enum class Wrap { Repeat, Clamp };
  enum class Filter { Linear, Closest };

  Texture(const Filter &filter = Filter::Linear,
          const Wrap &wrap = Wrap::Repeat, bool generate_mipmaps = true);

  auto id() const -> int;

  bool generate_mipmaps;
  Filter filter;
  Wrap wrap;
  TimePoint modified;

private:
  static std::atomic_int current_id_;
  int id_;
};
} // namespace mos::gfx
