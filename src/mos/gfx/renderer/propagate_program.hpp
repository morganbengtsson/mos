#pragma once

#include <glad/glad.h>

/** Uniforms for the propagate shader. */
class Propagate_program : public Program {
public:
  Propagate_program();
  GLint environment_sampler;
  GLint environment_albedo_sampler;
  GLint side;
};
