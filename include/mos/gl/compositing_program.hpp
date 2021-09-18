#pragma once

#include <glad/glad.h>

namespace mos::gl {

class Compositing_program : public Program {
  friend class Renderer;
private:
  Compositing_program();
public:
  GLint color_sampler;
  GLint bloom_sampler;
  GLint bloom_strength;
};
}
