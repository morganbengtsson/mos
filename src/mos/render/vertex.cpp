#include <mos/render/vertex.hpp>

namespace mos {

Vertex::Vertex(const glm::vec3 position, const glm::vec3 normal, const glm::vec3 tangent,
               const glm::vec2 uv, const glm::vec2 uv_lightmap)
    : position(position), normal(normal), tangent(tangent), uv(uv), uv_lightmap(uv_lightmap) {}

Vertex::~Vertex() {}

Vertex Vertex::operator+(const Vertex &vertex) const {
  return Vertex(position + vertex.position, normal + vertex.normal, tangent + vertex.tangent,
                uv + vertex.uv, uv_lightmap + vertex.uv_lightmap);
}

Vertex Vertex::operator-(const Vertex &vertex) const {
  return Vertex(position - vertex.position, normal - vertex.normal, tangent - vertex.tangent,
                uv - vertex.uv, uv_lightmap - vertex.uv_lightmap);
}

Vertex Vertex::operator*(const Vertex &vertex) const {
  return Vertex(position * vertex.position, normal * vertex.normal, tangent * vertex.tangent,
                uv * vertex.uv, uv_lightmap * vertex.uv_lightmap);
}

Vertex Vertex::operator*(const float number) const {
  return Vertex(position * number, normal * number, tangent * number, uv * number,
                uv_lightmap * number);
}

Vertex Vertex::operator/(const float number) const {
  return Vertex(position / number, normal / number, tangent / number, uv / number,
                uv_lightmap / number);
}

Vertex Vertex::operator/(const Vertex &vertex) const {
  return Vertex(position / vertex.position, normal / vertex.normal, tangent / vertex.tangent,
                uv / vertex.uv, uv_lightmap / vertex.uv_lightmap);
}
}
