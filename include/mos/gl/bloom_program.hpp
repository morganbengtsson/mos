#pragma once

#include <glad/glad.h>

namespace mos::gfx::gl {

struct Bloom_program : public Program {
  Bloom_program();
  GLint color_sampler;
};
}
