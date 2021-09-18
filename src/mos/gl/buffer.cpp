#include <mos/gl/renderer.hpp>

namespace mos::gfx::gl {

Buffer::Buffer(GLenum type, GLsizeiptr size, const void *data,
                         GLenum hint, Time_point modified)
    : id(Renderer::generate(glGenBuffers)), modified(modified) {
  glBindBuffer(type, id);
  glBufferData(type, size, data, hint);
  glBindBuffer(type, 0);
}

Buffer::Buffer(Buffer &&buffer) noexcept : id(buffer.id) {
  buffer.id = 0;
}

auto
Buffer::operator=(Buffer &&buffer) noexcept -> Buffer & {
  if (this != &buffer) {
    release();
    std::swap(id, buffer.id);
  }
  return *this;
}

Buffer::~Buffer() { release(); }

void Buffer::release() {
  glDeleteBuffers(1, &id);
  id = 0;
}
} // namespace mos::gfx
