#pragma once

#include <atomic>

namespace mos {
namespace gfx {

class Shape {
public:
  Shape();
  unsigned int id() const;
private:
  static std::atomic_uint current_id_;
  unsigned int id_;
};

}
}



