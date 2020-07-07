#pragma once

#include <glad/glad.h>

/** Uniforms for the environment shader. */
class Environment_program : public Program {
public:
  Environment_program();
  GLint model_view_projection;
  GLint model_matrix;
  GLint normal_matrix;
  std::array<GLint, 4> depth_bias_mvps{};

  struct Environment_uniforms {
    GLint map;
    GLint position;
    GLint extent;
    GLint strength;
  };

  GLint material_albedo_sampler;
  GLint material_emission_sampler;
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
  GLint camera_near;
  GLint camera_far;

  std::array<GLuint, 4> shadow_samplers{};
  std::array<Light_uniforms, 4> lights{};

  GLint fog_color_near;
  GLint fog_color_far;
  GLint fog_attenuation_factor;
  GLint fog_min;
  GLint fog_max;

  GLint brdf_lut;
};
