#pragma once

#include <array>
#include "../../src/mos/gfx/renderer/program.hpp"
#include "../../src/mos/gfx/renderer/shader.hpp"

/** Uniforms for the particle shader program. */
struct Cloud_program : public Program {

  struct Environment_uniforms {
    GLint map;
    GLint position;
    GLint extent;
    GLint strength;
    GLint falloff;
  };

  struct Camera_uniforms {
    GLint position;
    GLint near;
    GLint far;
  };

  struct Light_uniforms {
    GLint position;
    GLint color;
    GLint strength;
    GLint view;
    GLint projection;
    GLint angle;
    GLint direction;
    GLint blend;
  };

  struct Directional_light_uniforms {
    GLint position;
    GLint direction;
    GLint color;
    GLint strength;
  };



  explicit Cloud_program(const std::string &name,
                         const Shader &functions_shader);

  GLint model_view_projection;
  GLint model_view;
  GLint model{};
  GLint projection;

  GLint albedo_sampler;
  GLint emissive_sampler;

  GLint resolution;

  Camera_uniforms camera;

  std::array<Environment_uniforms, 2> environment_samplers{};
  std::array<Light_uniforms, 4> spot_lights{};
  Directional_light_uniforms directional_light;
};
