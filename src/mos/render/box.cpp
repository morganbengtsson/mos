#include <mos/render/box.hpp>
namespace mos {
namespace gfx {
Box::Box(const glm::vec3 &position, const glm::vec3 &extent)
    : position(position), extent(extent) {}

glm::vec3 Box::size() const {
  return glm::vec3(glm::abs(max().x - min().x), glm::abs(max().y - min().y),
                   glm::abs(max().z - min().z));
}

glm::vec3 Box::min() const { return position - extent; }
glm::vec3 Box::max() const { return position + extent; }
}
}
