#include <mos/gfx/gl/renderer.hpp>

namespace mos::gfx {

Renderer::Environment_map_target::Environment_map_target(
    const Renderer::Render_buffer &render_buffer)
    : frame_buffer(generate(glGenFramebuffers)),
      texture(generate(glGenTextures)), albedo(generate(glGenTextures)) {
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

  glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  static constexpr int num_textures = 6;
  for (int i = 0; i < num_textures; i++) {
    std::vector<unsigned char> data;
    for (int i = 0;
         i < render_buffer.resolution().x * render_buffer.resolution().y; i++) {
      data.push_back(0);
      data.push_back(0);
      data.push_back(0);
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                 render_buffer.resolution().x, render_buffer.resolution().y, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data.data());
  }
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_CUBE_MAP_POSITIVE_X, texture, 0);

  // Albedo target
  glBindTexture(GL_TEXTURE_CUBE_MAP, albedo);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  for (int i = 0; i < num_textures; i++) {
    std::vector<unsigned char> data;
    for (int i = 0;
         i < render_buffer.resolution().x * render_buffer.resolution().y; i++) {
      data.push_back(0);
      data.push_back(0);
      data.push_back(0);
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                 render_buffer.resolution().x, render_buffer.resolution().y, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data.data());
  }
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                         GL_TEXTURE_CUBE_MAP_POSITIVE_X, albedo, 0);

  std::array<GLuint, 2> attachments{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, attachments.data());

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, render_buffer.id);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Framebuffer incomplete.");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
Renderer::Environment_map_target::~Environment_map_target() {
  glDeleteTextures(1, &texture);
  glDeleteFramebuffers(1, &frame_buffer);
}
} // namespace mos::gfx
