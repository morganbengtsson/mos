#pragma once

#include <glad/glad.h>
#include <array>

/** Uniforms for the environment shader. */
class Environment_program : public Program {
public:

  struct Environment_uniforms {
    GLint map;
    GLint position;
    GLint extent;
    GLint strength;
  };

  struct Material_uniforms {
    GLint albedo_sampler;
    GLint emission_sampler;
    GLint albedo;
    GLint roughness;
    GLint metallic;
    GLint index_of_refraction;
    GLint alpha;
    GLint transmission;
    GLint emission;
    GLint ambient_occlusion;
  };

  struct Camera_uniforms {
    GLint position;
    GLint near;
    GLint far;
  };

  struct Fog_uniforms {
    GLint color_near;
    GLint color_far;
    GLint attenuation_factor;
    GLint min;
    GLint max;
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

  Environment_program();

  GLint model_view_projection;
  GLint model_matrix;
  GLint normal_matrix;

  std::array<GLint, 4> depth_bias_mvps{};

  Material_uniforms material;
  Fog_uniforms fog;
  Camera_uniforms camera;

  std::array<GLuint, 4> shadow_samplers{};
  std::array<Light_uniforms, 4> spot_lights{};

  //TODO: Add shadows, or remove shadows from spotlight
  Directional_light_uniforms directional_light;

  GLint brdf_lut_sampler;
};
