#include <mos/render/vertex.hpp>

namespace mos {
namespace gfx {

Vertex::Vertex(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec3 &tangent,
               const glm::vec2 &uv, const float ao)
    : position(position), normal(normal), tangent(tangent), uv(uv), ao(ao) {}

Vertex::~Vertex() {}

Vertex Vertex::operator+(const Vertex &vertex) const {
  return Vertex(position + vertex.position, normal + vertex.normal, tangent + vertex.tangent,
                uv + vertex.uv, ao + vertex.ao);
}

Vertex Vertex::operator-(const Vertex &vertex) const {
  return Vertex(position - vertex.position, normal - vertex.normal, tangent - vertex.tangent,
                uv - vertex.uv, ao - vertex.ao);
}

Vertex Vertex::operator*(const Vertex &vertex) const {
  return Vertex(position * vertex.position, normal * vertex.normal, tangent * vertex.tangent,
                uv * vertex.uv, ao * vertex.ao);
}

Vertex Vertex::operator*(const float number) const {
  return Vertex(position * number, normal * number, tangent * number, uv * number, ao * number);
}

Vertex Vertex::operator/(const float number) const {
  return Vertex(position / number, normal / number, tangent / number, uv / number, ao / number);
}

Vertex Vertex::operator/(const Vertex &vertex) const {
  return Vertex(position / vertex.position, normal / vertex.normal, tangent / vertex.tangent,
                uv / vertex.uv, ao / vertex.ao);
}
}
}
