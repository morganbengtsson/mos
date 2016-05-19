#include <mos/render/vertex.hpp>

namespace mos {

Vertex::Vertex(const glm::vec3 position, const glm::vec3 normal,
               const glm::vec2 uv, const glm::vec2 uv_lightmap)
    : position(position), normal(normal), uv(uv), uv_lightmap(uv_lightmap) {}
Vertex::Vertex(const float position_x, const float position_y,
               const float position_z, const float normal_x,
               const float normal_y, const float normal_z, const float uv_x,
               const float uv_y, const float uv_lightmap_x,
               const float uv_lightmap_y)
    : Vertex(glm::vec3(position_x, position_y, position_z),
             glm::vec3(normal_x, normal_y, normal_z), glm::vec2(uv_x, uv_y),
             glm::vec2(uv_lightmap_x, uv_lightmap_y)) {}

Vertex::~Vertex() {}

Vertex Vertex::operator+(const Vertex &vertex) const {
  return Vertex(position + vertex.position, normal + vertex.normal,
                uv + vertex.uv, uv_lightmap + vertex.uv_lightmap);
}

Vertex Vertex::operator-(const Vertex &vertex) const {
  return Vertex(position - vertex.position, normal - vertex.normal,
                uv - vertex.uv, uv_lightmap - vertex.uv_lightmap);
}

Vertex Vertex::operator*(const Vertex &vertex) const {
  return Vertex(position * vertex.position, normal * vertex.normal,
                uv * vertex.uv, uv_lightmap * vertex.uv_lightmap);
}

Vertex Vertex::operator*(const float number) const {
  return Vertex(position * number, normal * number, uv * number,
                uv_lightmap * number);
}

Vertex Vertex::operator/(const float number) const {
  return Vertex(position / number, normal / number, uv / number,
                uv_lightmap / number);
}

Vertex Vertex::operator/(const Vertex &vertex) const {
  return Vertex(position / vertex.position, normal / vertex.normal,
                uv / vertex.uv, uv_lightmap / vertex.uv_lightmap);
}
}
