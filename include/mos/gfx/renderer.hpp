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

/** Render geometry shapes with OpenGL. */
class Renderer final {
public:
  /** Inits the renderer, creates an OpenGL context with GLAD. */
  Renderer(const glm::vec4 &color, const glm::ivec2 &resolution);

  ~Renderer();

  /** Loads a model into renderers own memory. */
  void load(const Model &model);

  void load(const Models & models);

  /** Unloads a model from renderers own memory. */
  void unload(const Model &model);

  /** Load a mesh in to memory. */
  void load(const Mesh &mesh);
  void load(const Shared_mesh &mesh);

  /** Unloads a mesh from memory. */
  void unload(const Mesh &mesh);
  void unload(const Shared_mesh &mesh);

  /** Loads a shared texture into renderer memory. */
  void load(const Shared_texture_2D &texture);

  /** Loads a texture into renderer memory. */
  void load_or_update(const Texture_2D &texture);

  /** Unloads a shared texture from renderer memory. */
  void unload(const Shared_texture_2D &texture);

  /** Render multiple scenes. */
  void render(const Scenes &scenes,
              const glm::vec4 &color = {.0f, .0f, .0f, 1.0f},
              const glm::ivec2 &resolution = glm::ivec2(128, 128));

  /** Clear all internal buffers/memory. */
  void clear_buffers();

private:
  using Time_point =  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

  struct Buffer {
    GLuint id;
    Time_point modified;
  };

  class Texture_buffer_2D {
  public:
    explicit Texture_buffer_2D(const Texture_2D &texture_2d);
    Texture_buffer_2D(GLuint internal_format,
                    GLuint external_format,
                    int width,
                    int height,
                    GLuint wrap,
                    const void *data,
                    bool mipmaps,
                    const Time_point &modified = std::chrono::system_clock::now());
    ~Texture_buffer_2D();
    GLuint texture;
    Time_point modified;
  };

  class Shader {
  public:
    Shader(const std::string &source, GLuint type, const std::string& name);
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

  struct Light_uniforms {
    GLint position;
    GLint color;
    GLint strength;
    GLint view;
    GLint projection;
    GLint angle;
    GLint direction;
  };

  /** Uniforms for the particle shader program. */
  struct Particle_program : public Program {
    Particle_program();
    GLint mvp;
    GLint mv;
    GLint m;
    GLint p;
    GLint texture;
    GLint resolution;    
    GLint camera_resolution;
    GLint camera_position;
    std::array<Light_uniforms, 4> lights;
  };

  /** Uniforms for the bounding box shader program. */
  struct Box_program : public Program {
    Box_program();
    GLint mvp;
  };

  struct Multisample_program : public Program {
    Multisample_program();
    GLint color_texture;
    GLint depth_texture;
  };

  struct Ambient_occlusion_program : public Program {
    Ambient_occlusion_program();
    GLint ambient_sampler_uniform;
    GLint depth_sampler_uniform;
  };

  struct Bloom_program : public Program {
    Bloom_program();
    GLint direct_sampler_uniform;
    GLint ambient_sampler_uniform;
    GLint bloom_sampler_uniform;
    GLint strength;
  };

  struct Blur_program : public Program {
    Blur_program();
    GLint color_texture;
    GLint horizontal;
  };

  struct Depth_program : public Program {
    Depth_program();
    GLint model_view_projection_matrix;
  };


  /** Uniforms for the propagate shader. */
  class Propagate_program : public Program {
  public:
    Propagate_program();
    GLint environment_map;
    GLint environment_albedo_map;
    GLint side;
  };

  /** Uniforms for the environment shader. */
  class Environment_program : public Program {
  public:
    Environment_program();
    GLint model_view_projection_matrix;
    GLint model_matrix;
    GLint normal_matrix;
    std::array<GLint, 4> depth_bias_mvps;

    struct Environment_uniforms {
      GLint map;
      GLint position;
      GLint extent;
      GLint strength;
    };

    GLint material_albedo_map;
    GLint material_emission_map;
    GLint material_albedo;
    GLint material_roughness;
    GLint material_metallic;
    GLint material_opacity;
    GLint material_transmission;
    GLint material_emission;
    GLint material_ambient_occlusion;
    GLint material_strength;
    GLint material_factor;

    GLint camera_resolution;
    GLint camera_position;

    std::array<GLuint,4> shadow_maps;
    std::array<Light_uniforms, 4> lights;

    GLint fog_color_near;
    GLint fog_color_far;
    GLint fog_attenuation_factor;

    GLint brdf_lut;
  };

  /** Uniforms for the standard shader. */
  class Standard_program : public Program {
  public:
    Standard_program();
    GLint model_view_projection_matrix;
    GLint model_matrix;
    GLint normal_matrix;
    std::array<GLint,4> depth_bias_mvps;

    struct Environment_uniforms {
      GLint map;
      GLint position;
      GLint extent;
      GLint strength;
    };
    std::array<Environment_uniforms, 2> environment_maps;

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
    GLint material_transmission;
    GLint material_emission;
    GLint material_ambient_occlusion;
    GLint material_strength;
    GLint material_factor;

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
    };

    std::array<GLuint, 4> shadow_maps;
    std::array<Light_uniforms, 4> lights;

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

  void render_particles(const Particle_clouds &clouds,
                        const Lights &lights,
                        const mos::gfx::Camera &camera,
                        const glm::ivec2 &resolution);

  void render_model(const Model &model,
                    const glm::mat4 &transform,
                    const Camera &camera,
                    const Lights &lights,
                    const Environment_lights &environment_lights,
                    const Fog &fog,
                    const glm::vec2 &resolution,
                    const Standard_program& program);

  void render_model(const Model &model,
                    const glm::mat4 &transform,
                    const Camera &camera,
                    const Lights &lights,
                    const Environment_lights &environment_lights,
                    const Fog &fog,
                    const glm::vec2 &resolution,
                    const Environment_program& program);

  void render_model_depth(const Model &model,
                          const glm::mat4 &transform,
                          const Camera &camera,
                          const glm::vec2 &resolution,
                          const Depth_program& program);

  /** Clear color and depth. */
  void clear(const glm::vec4 &color);
  void clear_depth();
  void clear_color(const glm::vec4 &color);

  const Propagate_program propagate_program_;
  const Standard_program standard_program_;
  const Environment_program environment_program_;
  const Particle_program particle_program_;
  const Box_program box_program_;
  const Depth_program depth_program_;
  const Multisample_program multisample_program_;
  const Bloom_program bloom_program_;
  const Blur_program blur_program_;

  std::unordered_map<unsigned int, GLuint> frame_buffers_;
  std::unordered_map<unsigned int, GLuint> render_buffers;
  std::unordered_map<unsigned int, std::unique_ptr<Texture_buffer_2D>> textures_;
  std::unordered_map<unsigned int, Buffer> array_buffers_;
  std::unordered_map<unsigned int, Buffer> element_array_buffers_;
  std::unordered_map<unsigned int, GLuint> vertex_arrays_;

  struct Standard_target {
    Standard_target(const glm::ivec2 &resolution);
    ~Standard_target();
    GLuint frame_buffer;
    GLuint direct_shading_texture;
    GLuint indirect_shading_texture;
    GLuint normals_texture;
    GLuint depth_texture;
  };

  const Standard_target standard_target_;

  struct Multi_target {
    Multi_target(const glm::ivec2 &resolution);
    ~Multi_target();
    GLuint frame_buffer;
    GLuint color_texture;
    GLuint bright_texture;
  };

  const Multi_target multi_target_;

  struct Blur_target {
    Blur_target(const glm::ivec2 &resolution);
    ~Blur_target();
    GLuint frame_buffer;
    GLuint texture;
  };

  const Blur_target blur_target0_;
  const Blur_target blur_target1_;

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

  const Texture_buffer_2D black_texture_;
  const Texture_buffer_2D white_texture_;
  const Texture_buffer_2D brdf_lut_texture_;

  std::array<int,2> cube_camera_index_;

  struct Render_buffer {
    explicit Render_buffer(int resolution);
    ~Render_buffer();
    GLuint render_buffer;
    int resolution;
  };

  struct Shadow_map_target {
    explicit Shadow_map_target(const Render_buffer &render_buffer);
    ~Shadow_map_target();
    GLuint texture;
    GLuint frame_buffer;
  };

  struct Shadow_map_blur_target{
    explicit Shadow_map_blur_target(const int &resolution);
    ~Shadow_map_blur_target();
    GLuint texture;
    GLuint frame_buffer;
  };

  /** Shadow maps. */
  const Render_buffer shadow_maps_render_buffer_;
  const std::array<Shadow_map_target, 4> shadow_maps_;
  const Shadow_map_blur_target shadow_map_blur_target_;
  const std::array<Shadow_map_blur_target, 4> shadow_map_blur_targets_;

  struct Environment_map_target {
    explicit Environment_map_target(const Render_buffer &render_buffer);
    ~Environment_map_target();
    GLuint texture;
    GLuint albedo;
    GLuint frame_buffer;
  };

  /** Environment map targets. */
  const Render_buffer environment_render_buffer_;
  const std::array<Environment_map_target, 2> environment_maps_targets;
  const Environment_map_target propagate_target_;
};
}
}
