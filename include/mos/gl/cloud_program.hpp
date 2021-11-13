#pragma once

#include <array>
#include <mos/gl/program.hpp>
#include <mos/gl/shader.hpp>

namespace mos::gl {

/** Uniforms for the particle shader program. */
class Cloud_program : public Program {
  friend class Renderer;
public:
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

private:
  explicit Cloud_program(const std::string &name,
                         const Shader &functions_shader);

public:
  GLint model_view_projection;
  GLint model_view;
  GLint model{};
  GLint projection;

  GLint albedo_sampler;
  GLint emissive_sampler;

  GLint resolution;

  Camera_uniforms camera{};

  std::array<Environment_uniforms, 2> environment_samplers{};
  std::array<Light_uniforms, 4> spot_lights{};
  Directional_light_uniforms directional_light{};
};
}
