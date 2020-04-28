#include <mos/gfx/vertex.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace mos::gfx {

Vertex::Vertex(const glm::vec3 &position,
               const glm::vec3 &normal,
               const glm::vec3 &tangent,
               const glm::vec2 &uv)
    : position(position),
      normal(normal),
      tangent(tangent),
      uv(uv) {}

void Vertex::apply_transform(const glm::mat4 &transform) {
  position = glm::vec3(transform * glm::vec4(position, 1.0f));
  const auto inv_transpose = glm::mat3(glm::inverseTranspose(transform));
  normal =  inv_transpose * normal;
  tangent = inv_transpose * tangent;
}
}
