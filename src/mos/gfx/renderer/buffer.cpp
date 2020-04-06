#include <mos/gfx/renderer.hpp>

namespace mos::gfx {

Renderer::Buffer::Buffer(GLenum type, GLsizeiptr size, const void *data,
                         GLenum hint, Time_point modified)
    : id(generate(glGenBuffers)), modified(modified) {
  glBindBuffer(type, id);
  glBufferData(type, size, data, hint);
  glBindBuffer(type, 0);
}

Renderer::Buffer::Buffer(Renderer::Buffer &&buffer) noexcept : id(buffer.id) {
  buffer.id = 0;
}

Renderer::Buffer &
Renderer::Buffer::operator=(Renderer::Buffer &&buffer) noexcept {
  if (this != &buffer) {
    release();
    std::swap(id, buffer.id);
  }
  return *this;
}

Renderer::Buffer::~Buffer() { release(); }

void Renderer::Buffer::release() {
  glDeleteBuffers(1, &id);
  id = 0;
}
} // namespace mos::gfx
