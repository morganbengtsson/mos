#pragma once

#include <glad/glad.h>

namespace mos::gl {

class Quad {
  friend class Renderer;
private:
  Quad();
  ~Quad();
public:
  Quad(const Quad &target) = delete;
  Quad(const Quad &&target) = delete;
  Quad &operator=(const Quad &target) = delete;
  Quad &operator=(const Quad &&target) = delete;
  const GLuint vertex_array{};
  const GLuint buffer{};
};
}
