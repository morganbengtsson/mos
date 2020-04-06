#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

struct Blit_target {
  explicit Blit_target(const glm::ivec2 &resolution, GLint precision = GL_RGB16F);
  ~Blit_target();
  Blit_target(const Blit_target &target) = delete;
  Blit_target(Blit_target &&target) = delete;
  Blit_target & operator=(const Blit_target &target) = delete;
  Blit_target & operator=(Blit_target &&target) = delete;
  const GLuint frame_buffer{};
  const GLuint texture{};
  const GLuint depth_texture{};
  const glm::ivec2 resolution;
};
