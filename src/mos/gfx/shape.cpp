#include <mos/gfx/shape.hpp>

namespace mos::gfx {

unsigned int Shape::current_id_(0);

mos::gfx::Shape::Shape(): id_(current_id_++){
}
auto Shape::id() const -> unsigned int {
  return id_;
}
}


