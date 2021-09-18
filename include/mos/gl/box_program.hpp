#pragma once

#include <glad/glad.h>

namespace mos::gl {

/** Uniforms for the bounding box shader program. */
class Box_program : public Program {
  friend class Renderer;
private:
  Box_program();
public:
  GLint model_view_projection;
};
}
