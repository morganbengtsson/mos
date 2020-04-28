#include <mos/gfx/renderer.hpp>

namespace mos::gfx {

Renderer::Render_buffer::Render_buffer(const glm::ivec2 &res)
    : id(generate(glGenRenderbuffers)) {
  glBindRenderbuffer(GL_RENDERBUFFER, id);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, res.x, res.y);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

Renderer::Render_buffer::Render_buffer(int resolution)
    : Render_buffer(glm::ivec2(resolution, resolution)) {}

Renderer::Render_buffer::~Render_buffer() { release(); }

Renderer::Render_buffer::Render_buffer(
    Render_buffer &&render_buffer) noexcept
    : id(render_buffer.id) {
  render_buffer.id = 0;
}

auto Renderer::Render_buffer::operator=(
    Render_buffer &&render_buffer) noexcept -> Render_buffer & {
  if (this != &render_buffer) {
    release();
    std::swap(id, render_buffer.id);
  }
  return *this;
}

auto Renderer::Render_buffer::resolution() const -> glm::ivec2 {
  glm::ivec2 res;
  glGetNamedRenderbufferParameteriv(id, GL_RENDERBUFFER_WIDTH, &res.x);
  glGetNamedRenderbufferParameteriv(id, GL_RENDERBUFFER_HEIGHT, &res.y);
  return res;
}

void Renderer::Render_buffer::release() {
  glDeleteRenderbuffers(1, &id);
  id = 0;
}

} // namespace mos::gfx
