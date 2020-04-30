#pragma once

#include <atomic>

namespace mos::gfx {

/** Base class for geometric shapes. */
class Shape {
public:
  Shape();
  /** Unique id. */
  auto id() const -> unsigned int;
private:
  static unsigned int current_id_;
  unsigned int id_;
};
}
