#include "vertex.hpp"

namespace mos{

Vertex::Vertex(const glm::vec3 position,
       const glm::vec3 normal,
       const glm::vec2 uv,
       const glm::vec2 uv_lightmap)
    : position(position),
      normal(normal),
      uv(uv),
      uv_lightmap(uv_lightmap) {

}
Vertex::Vertex(const float p1, const float p2, const float p3,
                const float n1, const float n2, const float n3,
                const float u1, const float u2,
                const float ul1, const float ul2)
    : Vertex(glm::vec3(p1, p2, p3),
             glm::vec3(n1, n2, n3),
             glm::vec2(u1, u2),
             glm::vec2(ul1, ul2)) {}

Vertex::~Vertex(){
}

Vertex Vertex::operator+(const Vertex & vertex) const {
    return Vertex(position + vertex.position, normal + vertex.normal, uv + vertex.uv, uv_lightmap + vertex.uv_lightmap);
}

Vertex Vertex::operator-(const Vertex & vertex) const {
    return Vertex(position - vertex.position, normal - vertex.normal, uv - vertex.uv, uv_lightmap - vertex.uv_lightmap);
}

Vertex Vertex::operator*(const Vertex & vertex) const {
    return Vertex(position * vertex.position, normal * vertex.normal, uv * vertex.uv, uv_lightmap * vertex.uv_lightmap);
}

Vertex Vertex::operator*(const float number) const {
    return Vertex(position * number, normal * number, uv * number, uv_lightmap * number);
}

Vertex Vertex::operator/(const float number) const {
    return Vertex(position / number, normal / number, uv / number, uv_lightmap / number);
}

Vertex Vertex::operator/(const Vertex & vertex) const {
    return Vertex(position / vertex.position, normal / vertex.normal, uv / vertex.uv, uv_lightmap / vertex.uv_lightmap);
}

}
