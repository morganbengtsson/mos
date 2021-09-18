#pragma once

#include <glad/glad.h>

namespace mos::gl {

struct Bloom_program : public Program {
  friend class Renderer;
private:
  Bloom_program();
public:
  GLint color_sampler;
};
}
