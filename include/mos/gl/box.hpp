#pragma once

#include <glad/glad.h>

namespace mos::gl {

class Box {
  friend class Renderer;
private:
  Box();
  ~Box();
public:
  Box(const Box &target) = delete;
  Box(const Box &&target) = delete;
  Box &operator=(const Box &target) = delete;
  Box &operator=(const Box &&target) = delete;
  const GLuint buffer{};
  const GLuint element_buffer{};
  const GLuint vertex_array{};
};
}
