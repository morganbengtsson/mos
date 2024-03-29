#pragma once

#include <glad/glad.h>

namespace mos::gl {

class Render_buffer {
  friend class Renderer;
  friend class Frame_buffer;
private:
  explicit Render_buffer(int resolution);
  explicit Render_buffer(const glm::ivec2 &resolution);
public:
  ~Render_buffer();
  Render_buffer(Render_buffer &&buffer) noexcept;
  Render_buffer(const Render_buffer &buffer) = delete;
  Render_buffer &operator=(const Render_buffer &buffer) = delete;
  Render_buffer &operator=(Render_buffer &&buffer) noexcept;
  GLuint id{0};
  auto resolution() const -> glm::ivec2;

private:
  void release();
};
}
