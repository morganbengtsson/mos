#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace mos::gl {

class Post_target {
  friend class Renderer;
private:
  explicit Post_target(const glm::ivec2 &resolution,
                       GLint precision = GL_RGB16F);
public:
  ~Post_target();
  Post_target(const Post_target &target) = delete;
  Post_target(Post_target &&target) = delete;
  auto operator=(const Post_target &target) -> Post_target & = delete;
  auto operator=(Post_target &&target) -> Post_target & = delete;
  const GLuint frame_buffer{};
  const GLuint texture{};
  const glm::ivec2 resolution;
};
} // namespace mos::gl
