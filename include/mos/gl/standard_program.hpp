#pragma once

#include <glad/glad.h>
#include <array>
#include "../gl/shader.hpp"

namespace mos::gfx::gl {

/** Uniforms for the standard shader. */
class Standard_program : public Program {
public:
  struct Material_uniforms {
    GLint albedo_sampler;
    GLint normal_sampler;
    GLint metallic_sampler;
    GLint roughness_sampler;
    GLint emission_sampler;
    GLint ambient_occlusion_sampler;
    GLint albedo;
    GLint roughness;
    GLint metallic;
    GLint index_of_refraction;
    GLint alpha;
    GLint transmission;
    GLint emission;
    GLint ambient_occlusion;
  };

  struct Environment_uniforms {
    GLint sampler;
    GLint position;
    GLint extent;
    GLint strength;
    GLint falloff;
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

  struct Camera_uniforms {
    GLuint resolution;
    GLuint position;
  };

  struct Fog_uniforms {
    GLint color_near;
    GLint color_far;
    GLint attenuation_factor;
    GLint min;
    GLint max;
  };

  explicit Standard_program(const Shader &functions_shader);

  GLint model_view_projection;
  GLint model_matrix;
  GLint normal_matrix;

  Material_uniforms material;
  Fog_uniforms fog;
  Camera_uniforms camera;

  std::array<Environment_uniforms, 2> environment_maps{};

  std::array<GLuint, 4> shadow_samplers{};
  std::array<Light_uniforms, 4> spot_lights{};
  std::array<GLint, 4> depth_bias_mvps{};

  Directional_light_uniforms directional_light;
  std::array<GLuint, 4> cascaded_shadow_samplers{};
  std::array<GLint, 4> cascaded_depth_bias_mvps{};
  GLint cascade_splits;

  GLint brdf_lut_sampler;
};
}
