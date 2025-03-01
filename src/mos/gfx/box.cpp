#include <mos/gfx/box.hpp>
namespace mos::gfx {
Box::Box(const glm::mat4 &transform, const glm::vec3 &extent)
    : transform(transform), extent(extent) {}

auto Box::size() const -> glm::vec3 {
  return {glm::abs(max().x - min().x), glm::abs(max().y - min().y),
          glm::abs(max().z - min().z)};
}

auto Box::min() const -> glm::vec3 { return glm::vec3(transform[3]) - extent; }
auto Box::max() const -> glm::vec3 { return glm::vec3(transform[3]) + extent; }
auto Box::inside(const glm::vec3 &point) const -> bool {
  auto mi = min();
  auto ma = max();
  return (mi.x <= point.x
      && point.x <= ma.x
      && mi.y <= point.y
      && point.y <= ma.y
      && mi.z <= point.z
      && point.z <= ma.z);
}
auto Box::position() const -> glm::vec3 {
  return glm::vec3(transform[3]);
}

Box::Box() :
    transform(glm::mat4(1.0F)),
    extent(0.0F) {
}
} // namespace mos::gfx
