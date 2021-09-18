#include <mos/gl/renderer.hpp>

namespace mos::gfx::gl {

Render_buffer::Render_buffer(const glm::ivec2 &resolution)
    : id(Renderer::generate(glGenRenderbuffers)) {
  glBindRenderbuffer(GL_RENDERBUFFER, id);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resolution.x, resolution.y);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

Render_buffer::Render_buffer(int resolution)
    : Render_buffer(glm::ivec2(resolution, resolution)) {}

Render_buffer::~Render_buffer() { release(); }

Render_buffer::Render_buffer(
    Render_buffer &&buffer) noexcept
    : id(buffer.id) {
  buffer.id = 0;
}

auto Render_buffer::operator=(
    Render_buffer &&buffer) noexcept -> Render_buffer & {
  if (this != &buffer) {
    release();
    std::swap(id, buffer.id);
  }
  return *this;
}

auto Render_buffer::resolution() const -> glm::ivec2 {
  glm::ivec2 res;
  glGetNamedRenderbufferParameteriv(id, GL_RENDERBUFFER_WIDTH, &res.x);
  glGetNamedRenderbufferParameteriv(id, GL_RENDERBUFFER_HEIGHT, &res.y);
  return res;
}

void Render_buffer::release() {
  glDeleteRenderbuffers(1, &id);
  id = 0;
}
} // namespace mos::gfx
