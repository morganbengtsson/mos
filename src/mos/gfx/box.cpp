#include <mos/gfx/box.hpp>
namespace mos {
namespace gfx {
Box::Box(const glm::mat4 &transform, const glm::vec3 &extent)
    : transform(transform), extent(extent) {}

glm::vec3 Box::size() const {
  return glm::vec3(glm::abs(max().x - min().x), glm::abs(max().y - min().y),
                   glm::abs(max().z - min().z));
}

glm::vec3 Box::min() const { return glm::vec3(transform[3]) - extent; }
glm::vec3 Box::max() const { return glm::vec3(transform[3]) + extent; }
bool Box::inside(const glm::vec3 &point) const {
  auto mi = min();
  auto ma = max();
  return (mi.x <= point.x
      && point.x <= ma.x
      && mi.y <= point.y
      && point.y <= ma.y
      && mi.z <= point.z
      && point.z <= ma.z);
}
glm::vec3 Box::position() const {
  return glm::vec3(transform[3]);
}
}
}
