#pragma once

#include <glad/glad.h>

namespace mos::gl {

class Depth_program : public Program {
  friend class Renderer;
private:
  Depth_program();
public:
  GLint model_view_projection;
  GLint albedo_sampler;
  GLint albedo;
  GLint emission;
};
}
