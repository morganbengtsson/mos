#pragma once

#include <glad/glad.h>

/** Uniforms for the standard shader. */
class Standard_program : public Program {
public:
  explicit Standard_program(const Shader &functions_shader);
  GLint model_view_projection;
  GLint model_matrix;
  GLint normal_matrix;
  std::array<GLint, 4> depth_bias_mvps{};

  struct Environment_uniforms {
    GLint map;
    GLint position;
    GLint extent;
    GLint strength;
    GLint falloff;
  };
  std::array<Environment_uniforms, 2> environment_maps{};

  GLint material_albedo_sampler;
  GLint material_normal_sampler;
  GLint material_metallic_sampler;
  GLint material_roughness_sampler;
  GLint material_emission_sampler;
  GLint material_ambient_occlusion_sampler;
  GLint material_albedo;
  GLint material_roughness;
  GLint material_metallic;
  GLint material_index_of_refraction;
  GLint material_alpha;
  GLint material_transmission;
  GLint material_emission;
  GLint material_ambient_occlusion;

  GLint camera_resolution;
  GLint camera_position;

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

  std::array<GLuint, 4> shadow_maps{};
  std::array<Light_uniforms, 4> lights{};

  Directional_light_uniforms directional_light;

  GLint fog_color_near;
  GLint fog_color_far;
  GLint fog_attenuation_factor;
  GLint fog_min;
  GLint fog_max;

  GLint brdf_lut;
};
