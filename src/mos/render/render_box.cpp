#include <mos/render/render_box.hpp>
namespace mos {
RenderBox::RenderBox(const glm::vec3 &position, const glm::vec3 &extent)
    : position(position), extent(extent) {}

glm::vec3 RenderBox::size() const {
  return glm::vec3(glm::abs(max().x - min().x), glm::abs(max().y - min().y),
                   glm::abs(max().z - min().z));
}

glm::vec3 RenderBox::min() const { return position - extent; }
glm::vec3 RenderBox::max() const { return position + extent; }
}
