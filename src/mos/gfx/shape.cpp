#include <mos/gfx/shape.hpp>

namespace mos {
namespace gfx {

unsigned int Shape::current_id_(0);

mos::gfx::Shape::Shape(): id_(current_id_++){
}
unsigned int Shape::id() const {
  return id_;
}
}
}


