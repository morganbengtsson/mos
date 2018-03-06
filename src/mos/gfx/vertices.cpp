#include <mos/gfx/vertices.hpp>

namespace mos{
namespace gfx{

Vertices::Container::iterator Vertices::begin() {
  return vertices_.begin();
}
Vertices::Container::iterator Vertices::end() {
  return vertices_.end();
}
Vertices::Container::const_iterator Vertices::begin() const {
  return vertices_.begin();
}
Vertices::Container::const_iterator Vertices::end() const {
  return vertices_.end();
}
}
}