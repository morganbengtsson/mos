#pragma once
#include <glad/glad.h>
#include <optional>
#include <initializer_list>
#include <unordered_map>
#include <array>
#include <future>
#include <mos/gfx/scene.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/model.hpp>
#include <mos/gfx/text.hpp>
#include <mos/gfx/particle_cloud.hpp>
#include <mos/gfx/light.hpp>
#include <mos/gfx/target.hpp>
#include <mos/gfx/camera.hpp>
#include <mos/gfx/environment_light.hpp>
#include <mos/gfx/fog.hpp>
#include <mos/gfx/box.hpp>
#include <mos/gfx/scenes.hpp>
#include <mos/gfx/lights.hpp>

namespace mos {
namespace gfx {

/** Render geometry/models with OpenGL. */
class Renderer final {
public:
  /** Inits the renderer, creates an OpenGL context with GLEW. */
  Renderer(const glm::vec4 &color, const glm::ivec2 &resolution);

  ~Renderer();

  /** Loads a model into renderers own memory. */
  void load(const Model &model);

  void load(const Models & models);

  /** Unloads a model from renderers own memory. */
  void unload(const Model &model);

  /** Load a mesh in to memory. */
  void load(const Mesh &mesh);
  void load(const SharedMesh &mesh);

  /** Unloads a mesh from memory. */
  void unload(const Mesh &mesh);
  void unload(const SharedMesh &mesh);

  /** Loads a shared texture into renderer memory. */
  void load(const SharedTexture2D &texture);

  /** Loads a texture into renderer memory. */
  void load_or_update(const Texture2D &texture);

  /** Unloads a shared texture from renderer memory. */
  void unload(const SharedTexture2D &texture);

  /** Render multiple scenes. */
  void render(const Scenes &scenes,
              const glm::vec4 &color = {.0f, .0f, .0f, 1.0f},
              const glm::ivec2 &resolution = glm::ivec2(128, 128));

  /** Clear all internal buffers/memory. */
  void clear_buffers();

private:
  using TimePoint =  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

  struct Buffer {
    GLuint id;
    TimePoint modified;
  };

  class TextureBuffer2D {
  public:
    explicit TextureBuffer2D(const Texture2D &texture_2d);
    TextureBuffer2D(const GLuint internal_format,
                    const GLuint external_format,
                    const int width,
                    const int height,
                    const GLuint wrap,
                    const void *data,
                    const bool mipmaps,
                    const TimePoint &modified = std::chrono::system_clock::now());
    ~TextureBuffer2D();
    GLuint texture;
    TimePoint modified;
  };

  class Shader {
  public:
    Shader(const std::string &source, const GLuint type, const std::string& name);
    ~Shader();
    GLuint id;
  };

  struct Program {
    Program();
    ~Program();
    GLuint program;
    void check(const std::string &name);
    void link(const std::string &name);
  };

  /** Uniforms for the particle shader program. */
  struct ParticleProgram : public Program {
    ParticleProgram();
    GLint mvp;
    GLint mv;
    GLint p;
    GLint texture;
    GLint resolution;
  };

  /** Uniforms for the bounding box shader program. */
  struct BoxProgram : public Program {
    BoxProgram();
    GLint mvp;
  };

  struct MultisampleProgram : public Program {
    MultisampleProgram();
    GLint color_texture;
    GLint depth_texture;
  };

  struct BloomProgram : public Program {
    BloomProgram();
    GLint color_texture;
    GLint bright_color_texture;
    GLint strength;
  };

  struct BlurProgram : public Program {
    BlurProgram();
    GLint color_texture;
    GLint horizontal;
  };

  struct DepthProgram : public Program {
    DepthProgram();
    GLint model_view_projection_matrix;
  };


  /** Uniforms for the propagate shader. */
  class PropagateProgram : public Program {
  public:
    PropagateProgram();
    GLint environment_map;
    GLint environment_albedo_map;
    GLint side;
  };

  /** Uniforms for the environment shader. */
  class EnvironmentProgram : public Program {
  public:
    EnvironmentProgram();
    GLint model_view_projection_matrix;
    GLint model_matrix;
    GLint normal_matrix;
    std::array<GLint,2> depth_bias_mvps;

    struct EnvironmentUniforms {
      GLint map;
      GLint position;
      GLint extent;
      GLint strength;
    };
    std::array<EnvironmentUniforms, 2> environment_maps;

    GLint material_albedo_map;
    GLint material_emission_map;
    GLint material_normal_map;
    GLint material_metallic_map;
    GLint material_roughness_map;
    GLint material_ambient_occlusion_map;
    GLint material_albedo;
    GLint material_roughness;
    GLint material_metallic;
    GLint material_opacity;
    GLint material_emission;
    GLint material_ambient_occlusion;
    GLint material_emission_strength;
    GLint material_factor;

    GLint camera_resolution;
    GLint camera_position;

    struct LightUniforms {
      GLint position;
      GLint color;
      GLint strength;
      GLint view;
      GLint projection;
      GLint angle;
      GLint direction;
    };

    std::array<GLuint, 2> shadow_maps;
    std::array<LightUniforms, 2> lights;

    GLint fog_color_near;
    GLint fog_color_far;
    GLint fog_attenuation_factor;

    GLint brdf_lut;
  };

  /** Uniforms for the standard shader. */
  class StandardProgram : public Program {
  public:
    StandardProgram();
    GLint model_view_projection_matrix;
    GLint model_matrix;
    GLint normal_matrix;
    std::array<GLint,2> depth_bias_mvps;

    struct EnvironmentUniforms {
      GLint map;
      GLint position;
      GLint extent;
      GLint strength;
    };
    std::array<EnvironmentUniforms, 2> environment_maps;

    GLint material_albedo_map;
    GLint material_emission_map;
    GLint material_normal_map;
    GLint material_metallic_map;
    GLint material_roughness_map;
    GLint material_ambient_occlusion_map;
    GLint material_albedo;
    GLint material_roughness;
    GLint material_metallic;
    GLint material_opacity;
    GLint material_emission;
    GLint material_ambient_occlusion;
    GLint material_emission_strength;
    GLint material_factor;

    GLint camera_resolution;
    GLint camera_position;

    struct LightUniforms {
      GLint position;
      GLint color;
      GLint strength;
      GLint view;
      GLint projection;
      GLint angle;
      GLint direction;
    };

    std::array<GLuint, 2> shadow_maps;
    std::array<LightUniforms, 2> lights;

    GLint fog_color_near;
    GLint fog_color_far;
    GLint fog_attenuation_factor;

    GLint brdf_lut;
  };

  void render_texture_targets(const Scene &scene);

  void render_scene(const Camera &camera,
                    const Scene &scene,
                    const glm::ivec2 &resolution);

  void render_shadow_maps(const Models &models,
                          const Lights &lights);

  void render_environment(const Scene &scene,
                          const glm::vec4 &clear_color);

  void render_boxes(const Boxes & boxes,
                    const mos::gfx::Camera &camera);

  void render_particles(const ParticleClouds &clouds,
                        const mos::gfx::Camera &camera,
                        const glm::vec2 &resolution);

  void render_model(const Model &model,
                    const glm::mat4 &transform,
                    const Camera &camera,
                    const Lights &lights,
                    const EnvironmentLights &environment_lights,
                    const Fog &fog,
                    const glm::vec2 &resolution,
                    const StandardProgram& program);

  void render_model(const Model &model,
                    const glm::mat4 &transform,
                    const Camera &camera,
                    const Lights &lights,
                    const EnvironmentLights &environment_lights,
                    const Fog &fog,
                    const glm::vec2 &resolution,
                    const EnvironmentProgram& program);

  void render_model_depth(const Model &model,
                          const glm::mat4 &transform,
                          const Camera &camera,
                          const glm::vec2 &resolution,
                          const DepthProgram& program);

  /** Clear color and depth. */
  void clear(const glm::vec4 &color);
  void clear_depth();
  void clear_color(const glm::vec4 &color);

  const PropagateProgram propagate_program_;
  const StandardProgram standard_program_;
  const EnvironmentProgram environment_program_;
  const ParticleProgram particle_program_;
  const BoxProgram box_program_;
  const DepthProgram depth_program_;
  const MultisampleProgram multisample_program_;
  const BloomProgram bloom_program_;
  const BlurProgram blur_program_;

  std::unordered_map<unsigned int, GLuint> frame_buffers_;
  std::unordered_map<unsigned int, GLuint> render_buffers;
  std::unordered_map<unsigned int, std::unique_ptr<TextureBuffer2D>> textures_;
  std::unordered_map<unsigned int, Buffer> array_buffers_;
  std::unordered_map<unsigned int, Buffer> element_array_buffers_;
  std::unordered_map<unsigned int, GLuint> vertex_arrays_;

  struct StandardTarget {
    StandardTarget(const glm::ivec2 &resolution);
    ~StandardTarget();
    GLuint frame_buffer;
    GLuint texture;
    GLuint depth_texture;
  };

  const StandardTarget standard_target_;

  struct MultiTarget {
    MultiTarget(const glm::ivec2 &resolution);
    ~MultiTarget();
    GLuint frame_buffer;
    GLuint color_texture;
    GLuint bright_texture;
  };

  const MultiTarget multi_target_;

  struct BlurTarget {
    BlurTarget(const glm::ivec2 &resolution);
    ~BlurTarget();
    GLuint frame_buffer;
    GLuint texture;
  };

  const BlurTarget blur_target0_;
  const BlurTarget blur_target1_;

  struct Quad {
    Quad();
    ~Quad();
    GLuint vertex_array;
    GLuint buffer;
  };

  const Quad quad_;

  struct Box {
    Box();
    ~Box();
    GLuint buffer;
    GLuint element_buffer;
    GLuint vertex_array;
  };

  const Box box;

  const TextureBuffer2D black_texture_;
  const TextureBuffer2D white_texture_;
  const TextureBuffer2D brdf_lut_texture_;

  std::array<int,2> cube_camera_index_;

  struct RenderBuffer {
    explicit RenderBuffer(const int resolution);
    ~RenderBuffer();
    GLuint render_buffer;
    int resolution;
  };

  struct ShadowMapTarget {
    ShadowMapTarget(const RenderBuffer &render_buffer);
    ~ShadowMapTarget();
    GLuint texture;
    GLuint frame_buffer;
  };

  /** Shadow maps. */
  const RenderBuffer shadow_maps_render_buffer_;
  const std::array<ShadowMapTarget, 2> shadow_maps_;

  struct EnvironmentMapTarget {
    EnvironmentMapTarget(const RenderBuffer &render_buffer);
    ~EnvironmentMapTarget();
    GLuint texture;
    GLuint albedo;
    GLuint frame_buffer;
  };

  /** Environment map targets. */
  const RenderBuffer environment_render_buffer_;
  const std::array<EnvironmentMapTarget, 2> environment_maps_targets;
  const EnvironmentMapTarget propagate_target_;

  static const std::map<const unsigned int, std::string> shader_types_;

};
}
}