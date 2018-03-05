#include <mos/gfx/vertex.hpp>

namespace mos {
namespace gfx {

Vertex::Vertex(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec3 &tangent,
               const glm::vec2 &uv, const float ao)
    : position(position), normal(normal), tangent(tangent), uv(uv), ao(ao) {}

Vertex::~Vertex() {}
}
}
