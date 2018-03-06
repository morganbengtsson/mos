#include <mos/gfx/vertices.hpp>

namespace mos{
namespace gfx{
Vertices::Vertices() {
}

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
unsigned long Vertices::size() const {
  return vertices_.size();
}
void Vertices::clear() {
  vertices_.clear();
}
Vertex &Vertices::operator[](Vertices::Container::size_type pos) {
  return vertices_[pos];
}
const Vertex *Vertices::data() const noexcept {
  return vertices_.data();
}
void Vertices::push_back(const Vertex &vertex) {
  vertices_.push_back(vertex);
}
Vertex &Vertices::back() {
  return vertices_.back();
}
}
}