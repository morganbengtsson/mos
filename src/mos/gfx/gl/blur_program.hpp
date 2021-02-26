#pragma once

#include <glad/glad.h>

struct Blur_program : public Program {
  Blur_program();
  GLint color_sampler;
  GLint horizontal;
};
