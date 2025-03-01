#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace mos::gl {

class Blit_target {
  friend class Renderer;
private:
  explicit Blit_target(const glm::ivec2 &resolution,
                       GLint precision = GL_RGB16F);
public:
  ~Blit_target();
  Blit_target(const Blit_target &target) = delete;
  Blit_target(Blit_target &&target) = delete;
  auto operator=(const Blit_target &target) -> Blit_target & = delete;
  auto operator=(Blit_target &&target) -> Blit_target & = delete;
  const GLuint frame_buffer{};
  const GLuint texture{};
  const GLuint depth_texture{};
  const glm::ivec2 resolution;
};
} // namespace mos::gl
