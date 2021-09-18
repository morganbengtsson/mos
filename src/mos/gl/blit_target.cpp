#include <mos/gl/renderer.hpp>

namespace mos::gl {
Blit_target::Blit_target(const glm::ivec2 &resolution,
                                   const GLint precision)
    : frame_buffer(Renderer::generate(glGenFramebuffers)),
      texture(Renderer::generate(glGenTextures)),
      depth_texture(Renderer::generate(glGenTextures)),
      resolution(resolution) {
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, precision, resolution.x, resolution.y, 0,
               GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture, 0);

  glBindTexture(GL_TEXTURE_2D, depth_texture);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, resolution.x,
                 resolution.y);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depth_texture, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Framebuffer incomplete");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
Blit_target::~Blit_target() {
  glDeleteFramebuffers(1, &frame_buffer);
  glDeleteTextures(1, &texture);
  glDeleteTextures(1, &depth_texture);
}
} // namespace mos::gfx
