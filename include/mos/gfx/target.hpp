#pragma once

#include <atomic>
#include <memory>
#include <optional>

#include <glm/glm.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/core/id.hpp>

namespace mos::gfx {

class Target;
using Optional_target = std::optional<Target>;

/** Off screen rendering target. */
class Target final {
public:
  Target() = default;

  /** Unique id */
  auto id() const -> int;

private:
  Id<Target> id_;
};
} // namespace mos::gfx
