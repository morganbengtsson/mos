#pragma once

#include <glad/glad.h>

namespace mos::gfx::gl {

struct Depth_program : public Program {
  Depth_program();
  GLint model_view_projection;
  GLint albedo_sampler;
  GLint albedo;
  GLint emission;
};
}
