#pragma once

#include <glad/glad.h>

namespace mos::gl {

class Quad {
  friend class Renderer;
public:
  Quad();
  ~Quad();
  Quad(const Quad &target) = delete;
  Quad(const Quad &&target) = delete;
  auto operator=(const Quad &target) -> Quad & = delete;
  auto operator=(const Quad &&target) -> Quad & = delete;
  const GLuint vertex_array{};
  const GLuint buffer{};
};
} // namespace mos::gl
