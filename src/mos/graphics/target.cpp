#include <mos/graphics/target.hpp>

namespace mos {

unsigned int Target::current_id_ = 0;

Target::Target(const unsigned int width, const unsigned int height)
    : texture(std::make_shared<Texture>(Texture(width, height, true))) {
  id_ = current_id_++;
}
unsigned int Target::id() const { return id_; }
}
