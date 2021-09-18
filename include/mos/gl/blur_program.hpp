#pragma once

#include <glad/glad.h>

namespace mos::gfx::gl {

struct Blur_program : public Program {
  Blur_program();
  GLint color_sampler;
  GLint horizontal;
};
}
