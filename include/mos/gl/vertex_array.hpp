#pragma once

#include <mos/gfx/cloud.hpp>
#include <mos/gfx/mesh.hpp>

#include <mos/gl/buffer.hpp>

namespace mos::gl {

class Vertex_array {
  friend class Renderer;
private:
  explicit Vertex_array(
      const gfx::Cloud &cloud,
      std::unordered_map<unsigned int, Buffer> &array_buffers);
  explicit Vertex_array(
      const gfx::Mesh &mesh, std::unordered_map<unsigned int, Buffer> &array_buffers,
      std::unordered_map<unsigned int, Buffer> &element_array_buffers);
public:
  ~Vertex_array();
  Vertex_array(Vertex_array &&array) noexcept;
  Vertex_array(const Vertex_array &array) = delete;
  Vertex_array &operator=(const Vertex_array &array) = delete;
  Vertex_array &operator=(Vertex_array &&array) noexcept;
  GLuint id{0};

private:
  void release();
};

}
