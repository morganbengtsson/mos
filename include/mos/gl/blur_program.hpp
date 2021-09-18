#pragma once

#include <glad/glad.h>

namespace mos::gl {

class Blur_program : public Program {
  friend class Renderer;
private:
  Blur_program();
public:
  GLint color_sampler;
  GLint horizontal;
};
}
