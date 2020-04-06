#pragma once

#include <glad/glad.h>

class Render_buffer {
public:
  explicit Render_buffer(int resolution);
  explicit Render_buffer(const glm::ivec2 &resolution);
  ~Render_buffer();
  Render_buffer(Render_buffer &&target) noexcept;
  Render_buffer(const Render_buffer &target) = delete;
  Render_buffer &operator=(const Render_buffer &target) = delete;
  Render_buffer &operator=(Render_buffer &&target) noexcept;
  GLuint id{0};
  glm::ivec2 resolution() const;

private:
  void release();
};
