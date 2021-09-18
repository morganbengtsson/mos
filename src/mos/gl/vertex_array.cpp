#include <mos/gl/renderer.hpp>
#include <mos/gl/vertex_array.hpp>

namespace mos::gfx::gl {

Vertex_array::Vertex_array(
    const mos::gfx::Cloud &cloud,
    std::unordered_map<unsigned int, Buffer> &array_buffers) {
  glGenVertexArrays(1, &id);
  glBindVertexArray(id);
  if (array_buffers.find(cloud.id()) == array_buffers.end()) {
    array_buffers.insert(
        {cloud.id(),
         Buffer(GL_ARRAY_BUFFER, cloud.points.size() * sizeof(Point),
                cloud.points.data(), GL_STREAM_DRAW, cloud.points.modified())});
  }
  glBindBuffer(GL_ARRAY_BUFFER, array_buffers.at(cloud.id()).id);
  glVertexAttribPointer(
      0, decltype(Point::position)::length(), GL_FLOAT, GL_FALSE, sizeof(Point),
      reinterpret_cast<const void *>(offsetof(Point, position)));
  glVertexAttribPointer(1, decltype(Point::color)::length(), GL_FLOAT, GL_FALSE,
                        sizeof(Point),
                        reinterpret_cast<const void *>(offsetof(Point, color)));
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Point),
                        reinterpret_cast<const void *>(offsetof(Point, size)));
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Point),
                        reinterpret_cast<const void *>(offsetof(Point, alpha)));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glBindVertexArray(0);
}

Vertex_array::Vertex_array(
    const mos::gfx::Mesh &mesh,
    std::unordered_map<unsigned int, Buffer> &array_buffers,
    std::unordered_map<unsigned int, Buffer> &element_array_buffers) {
  glGenVertexArrays(1, &id);
  glBindVertexArray(id);
  if (array_buffers.find(mesh.id()) == array_buffers.end()) {
    array_buffers.insert(
        {mesh.id(),
         Buffer(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex),
                mesh.vertices.data(), GL_STATIC_DRAW,
                mesh.vertices.modified())});
  }
  if (element_array_buffers.find(mesh.id()) == element_array_buffers.end()) {
    element_array_buffers.insert(
        {mesh.id(), Buffer(GL_ELEMENT_ARRAY_BUFFER,
                           mesh.indices.size() * 3 * sizeof(unsigned int),
                           mesh.indices.data(), GL_STATIC_DRAW,
                           mesh.indices.modified())});
  }
  glBindBuffer(GL_ARRAY_BUFFER, array_buffers.at(mesh.id()).id);
  glVertexAttribPointer(
      0, decltype(Vertex::position)::length(), GL_FLOAT, GL_FALSE,
      sizeof(Vertex),
      reinterpret_cast<const void *>(offsetof(Vertex, position)));
  glVertexAttribPointer(
      1, decltype(Vertex::normal)::length(), GL_FLOAT, GL_FALSE, sizeof(Vertex),
      reinterpret_cast<const void *>(offsetof(Vertex, normal)));
  glVertexAttribPointer(
      2, decltype(Vertex::tangent)::length(), GL_FLOAT, GL_FALSE,
      sizeof(Vertex),
      reinterpret_cast<const void *>(offsetof(Vertex, tangent)));
  glVertexAttribPointer(3, decltype(Vertex::uv)::length(), GL_FLOAT, GL_FALSE,
                        sizeof(Vertex),
                        reinterpret_cast<const void *>(offsetof(Vertex, uv)));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffers.at(mesh.id()).id);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glBindVertexArray(0);
}

Vertex_array::~Vertex_array() { release(); }

Vertex_array::Vertex_array(Vertex_array &&array) noexcept
    : id(array.id) {
  array.id = 0;
}

auto
Vertex_array::operator=(Vertex_array &&array) noexcept -> Vertex_array & {
  if (this != &array) {
    release();
    std::swap(id, array.id);
  }
  return *this;
}

void Vertex_array::release() {
  glDeleteVertexArrays(1, &id);
  id = 0;
}
} // namespace mos::gfx
