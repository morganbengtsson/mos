#include <mos/gfx/gl/renderer.hpp>

namespace mos::gfx {

Renderer::Frame_buffer::Frame_buffer(
    const Texture_target &target,
    std::unordered_map<unsigned int, Texture_buffer_2D> &texture_buffers,
    std::unordered_map<unsigned int, Render_buffer> &render_buffers) {
  glGenFramebuffers(1, &id);
  glBindFramebuffer(GL_FRAMEBUFFER, id);

  texture_buffers.insert(
      {target.texture->id(), Texture_buffer_2D(*target.texture)});

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture_buffers.at(target.texture->id()).texture, 0);

  render_buffers.insert({target.target.id(),
                         Render_buffer(glm::ivec2(target.texture->width(),
                                                  target.texture->height()))});

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER,
                            render_buffers.at(target.target.id()).id);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Framebuffer incomplete.");
  }
}

Renderer::Frame_buffer::~Frame_buffer() { release(); }

Renderer::Frame_buffer::Frame_buffer(
    Renderer::Frame_buffer &&frame_buffer) noexcept
    : id(frame_buffer.id) {
  frame_buffer.id = 0;
}

auto Renderer::Frame_buffer::operator=(
    Frame_buffer &&frame_buffer) noexcept -> Frame_buffer & {
  if (this != &frame_buffer) {
    release();
    std::swap(id, frame_buffer.id);
  }
  return *this;
}

void Renderer::Frame_buffer::release() {
  glDeleteFramebuffers(1, &id);
  id = 0;
}
} // namespace mos::gfx
