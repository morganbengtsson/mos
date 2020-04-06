#pragma once

#include <glad/glad.h>

struct Box {
  Box();
  ~Box();
  Box(const Box &target) = delete;
  Box(const Box &&target) = delete;
  Box & operator=(const Box &target) = delete;
  Box & operator=(const Box &&target) = delete;
  const GLuint buffer{};
  const GLuint element_buffer{};
  const GLuint vertex_array{};
};
