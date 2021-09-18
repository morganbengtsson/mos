#include <mos/gl/renderer.hpp>

namespace mos::gfx::gl {

Shadow_map_target::Shadow_map_target(
    const Render_buffer &render_buffer)
    : frame_buffer(Renderer::generate(glGenFramebuffers)),
      texture(Renderer::generate(glGenTextures)) {
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, render_buffer.resolution().x,
               render_buffer.resolution().y, 0, GL_RG, GL_UNSIGNED_BYTE,
               nullptr);

  glBindTexture(GL_TEXTURE_2D, 0);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture, 0);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, render_buffer.id);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Framebuffer incomplete.");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
Shadow_map_target::~Shadow_map_target() {
  glDeleteFramebuffers(1, &frame_buffer);
  glDeleteTextures(1, &texture);
}
} // namespace mos::gfx
