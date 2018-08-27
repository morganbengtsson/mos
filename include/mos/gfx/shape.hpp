#pragma once

#include <atomic>

namespace mos {
namespace gfx {

/** Base class for geometric shapes. */
class Shape {
public:
  Shape();
  ~Shape() = default;
  /** Unique id. */
  unsigned int id() const;
private:
  static std::atomic_uint current_id_;
  unsigned int id_;
};

}
}



