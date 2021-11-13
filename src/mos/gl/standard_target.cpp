#include <mos/gl/renderer.hpp>

namespace mos::gl {

Standard_target::Standard_target(const glm::ivec2 &resolution,
                                           const int samples)
    : frame_buffer(Renderer::generate(glGenFramebuffers)),
      color_texture(Renderer::generate(glGenTextures)),
      depth_texture(Renderer::generate(glGenTextures)) {
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, color_texture);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA16F,
                          resolution.x, resolution.y, GL_TRUE);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D_MULTISAMPLE, color_texture, 0);

  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_texture);
  glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples,
                            GL_DEPTH_COMPONENT24, resolution.x, resolution.y,
                            1u);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                         GL_TEXTURE_2D_MULTISAMPLE, depth_texture, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Framebuffer incomplete.");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
Standard_target::~Standard_target() {
  glDeleteFramebuffers(1, &frame_buffer);
  glDeleteTextures(1, &color_texture);
  glDeleteTextures(1, &depth_texture);
}
} // namespace mos::gfx
