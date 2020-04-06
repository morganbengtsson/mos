#pragma once

#include <glad/glad.h>

struct Quad {
  Quad();
  ~Quad();
  Quad(const Quad &target) = delete;
  Quad(const Quad &&target) = delete;
  Quad & operator=(const Quad &target) = delete;
  Quad & operator=(const Quad &&target) = delete;
  const GLuint vertex_array{};
  const GLuint buffer{};
};
