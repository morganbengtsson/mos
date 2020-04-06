#pragma once

#include <mos/gfx/renderer/program.hpp>
#include <mos/gfx/renderer/shader.hpp>

/** Uniforms for the particle shader program. */
struct Cloud_program : public Program {
  explicit Cloud_program(const std::string &name, const Shader &functions_shader);
  GLint model_view_projection;
  GLint model_view;
  GLint model{};
  GLint projection;
  GLint texture;
  GLint resolution;
  GLint camera_resolution;
  GLint camera_position;
  GLint camera_far{};
  GLint camera_near{};

  struct Environment_uniforms {
    GLint map;
    GLint position;
    GLint extent;
    GLint strength;
    GLint falloff;
  };
  std::array<Environment_uniforms, 2> environment_maps {};
  std::array<Light_uniforms, 4> lights{};
};
