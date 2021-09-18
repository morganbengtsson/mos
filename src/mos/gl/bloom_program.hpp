#pragma once

#include <glad/glad.h>

struct Bloom_program : public Program {
  Bloom_program();
  GLint color_sampler;
};
