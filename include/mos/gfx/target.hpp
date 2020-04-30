#pragma once
#include <memory>
#include <atomic>
#include <optional>
#include <glm/glm.hpp>
#include <mos/gfx/texture_2d.hpp>

namespace mos::gfx {

class Target;
using Optional_target = std::optional<Target>;

/** Off screen rendering target. */
class Target final{
public:
  Target();

  /** Unique id */
  auto id() const -> unsigned int;
private:
  unsigned int id_;
  static std::atomic_uint current_id_;
};
}

