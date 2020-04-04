#include <mos/gfx/vertex.hpp>

namespace mos {
namespace gfx {

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
  normal = glm::vec3(transform * glm::vec4(normal, 1.0f)); //TODO: Not correct?
  tangent = glm::vec3(transform * glm::vec4(tangent, 1.0f)); //TODO: Not correct?
}
}
}
