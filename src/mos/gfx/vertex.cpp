#include <mos/gfx/vertex.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace mos::gfx {

void Vertex::apply_transform(const glm::mat4 &transform) {
  position = glm::vec3(transform * glm::vec4(position, 1.0F));
  const auto inv_transpose = glm::mat3(glm::inverseTranspose(transform));
  normal =  inv_transpose * normal;
  tangent = inv_transpose * tangent;
}
} // namespace mos::gfx
