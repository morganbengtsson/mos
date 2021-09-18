#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace mos::gfx::gl {

struct Post_target {
  explicit Post_target(const glm::ivec2 &resolution,
                       GLint precision = GL_RGB16F);
  ~Post_target();
  Post_target(const Post_target &target) = delete;
  Post_target(Post_target &&target) = delete;
  Post_target &operator=(const Post_target &target) = delete;
  Post_target &operator=(Post_target &&target) = delete;
  const GLuint frame_buffer{};
  const GLuint texture{};
  const glm::ivec2 resolution;
};
}
