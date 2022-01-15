#pragma once

#include <mos/core/id.hpp>

namespace mos::gfx {

/** Base class for geometric shapes. */
class Shape {
public:
  Shape() = default;
  /** Unique id. */
  auto id() const -> unsigned int;

private:
  Id<Shape> id_;
};
} // namespace mos::gfx
