#pragma once
#include <glad/glad.h>
#include <optional>
#include <initializer_list>
#include <unordered_map>
#include <array>
#include <future>
#include <mos/gfx/scene.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/texture_cube.hpp>
#include <mos/gfx/model.hpp>
#include <mos/gfx/text.hpp>
#include <mos/gfx/particle_cloud.hpp>
#include <mos/gfx/light.hpp>
#include <mos/gfx/target.hpp>
#include <mos/gfx/camera.hpp>
#include <mos/gfx/environment_light.hpp>
#include <mos/gfx/fog.hpp>
#include <mos/gfx/box.hpp>

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
  void load_async(const Model &model);

  void load(const Scene::Models & models);
  void load_async(const Scene::Models &models);

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

  /** Loads a shared texture into render memory, async. */
  void load_async(const SharedTexture2D &texture);

  /** Loads a texture into renderer memory. */
  void load_or_update(const Texture2D &texture);

  /** Unloads a shared texture from renderer memory. */
  void unload(const SharedTexture2D &texture);

  /** Unloads a shared texture cube from renderer memory.*/
  void unload(const SharedTextureCube &texture);

  void render(const std::initializer_list<Scene> &scenes_init,
              const glm::vec4 &color = glm::vec4(.0f),
              const glm::ivec2 &resolution = glm::ivec2(128, 128));

  void render_async(const std::initializer_list<Scene> &scenes_init,
              const glm::vec4 &color = glm::vec4(.0f),
              const glm::ivec2 &resolution = glm::ivec2(128, 128));


  template<class It>
  void render(It scenes_begin,
              It scenes_end,
              const glm::vec4 &color = {.0f, .0f, .0f, 1.0f},
              const glm::ivec2 &resolution = glm::ivec2(128, 128)) {
    clear(color);
    for (auto it = scenes_begin; it != scenes_end; it++) {
      load(it->models);
      render_shadow_map(it->models, it->light);
      render_environment(*it, color);
      render_texture_targets(*it);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, multi_fbo_);
    clear(color);
    for (auto it = scenes_begin; it != scenes_end; it++) {
      render_scene(it->camera, *it, resolution);
    }

    //RenderQuad
    glBindFramebuffer(GL_FRAMEBUFFER, color_fbo_);
    clear(color);
    glUseProgram(quad_program_.program);

    glBindVertexArray(quad_vao_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multi_texture_);
    glUniform1i(quad_program_.quad_texture, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multi_depth_texture_);
    glUniform1i(quad_program_.quad_depth_texture, 1);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glViewport(0, 0, resolution.x / 4, resolution.y / 4);
    for (int i = 0; i < 10; i++) {
      //Blur pass2
      glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo0_);
      glUseProgram(blur_program_.program);
      glBindVertexArray(quad_vao_);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, i == 0 ? bright_texture_ : blur_texture1_);
      glUniform1i(blur_program_.color_texture, 0);
      GLint horizontal = false;
      glUniform1iv(blur_program_.horizontal, 1, &horizontal);
      glDrawArrays(GL_TRIANGLES, 0, 6);


      //Blur pass3
      glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo1_);
      glUseProgram(blur_program_.program);
      glBindVertexArray(quad_vao_);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, blur_texture0_);
      glUniform1i(blur_program_.color_texture, 0);
      horizontal = true;
      glUniform1iv(blur_program_.horizontal, 1, &horizontal);
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glViewport(0, 0, resolution.x, resolution.y);
    //Render to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(bloom_program_.program);

    glBindVertexArray(quad_vao_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_texture0_);
    glUniform1i(bloom_program_.color_texture, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, blur_texture0_);
    glUniform1i(bloom_program_.bright_color_texture, 1);
    float strength = 0.3f;
    glUniform1fv(bloom_program_.strength, 1, &strength);

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  template<class It>
  void render_async(It scenes_begin,
              It scenes_end,
              const glm::vec4 &color = {.0f, .0f, .0f, 1.0f},
              const glm::ivec2 &resolution = glm::ivec2(128, 128)) {
    clear(color);
    for (auto it = scenes_begin; it != scenes_end; it++) {
      load_async(it->models);
      render_shadow_map(it->models, it->light);
      //render_environment(*it, color);
      render_scene(it->camera, *it, resolution);
    }
  }

  /** Clear all internal buffers/memory. */
  void clear_buffers();

private:
  void * ptr_;
  GLuint buffer_id_;

  GLuint multi_fbo_;
  GLuint multi_texture_;
  GLuint multi_depth_texture_;

  GLuint color_fbo_;
  GLuint color_texture0_;
  GLuint bright_texture_;

  GLuint blur_fbo0_;
  GLuint blur_fbo1_;
  GLuint blur_texture0_;
  GLuint blur_texture1_;

  GLuint quad_vao_;
  GLuint quad_vbo_;

  struct Buffer {
    GLuint id; // TODO const?
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> modified;
  };

  struct PixelBuffer {
    GLuint id;
    std::future<void> future;
  };

  /** Uniforms for the particle shader program. */
  struct ParticleProgram {
    ParticleProgram();
    ~ParticleProgram();
    GLuint program;
    GLint mvp;
    GLint mv;
    GLint p;
    GLint texture;
    GLint resolution;
  };

  /** Uniforms for the bounding box shader program. */
  struct BoxProgram {
    BoxProgram();
    ~BoxProgram();
    GLuint program;
    GLint mvp;
    GLint mv;
  };

  struct QuadProgram {
    QuadProgram();
    ~QuadProgram();
    GLuint program;
    GLint quad_texture;
    GLint quad_depth_texture;
  };

  struct BloomProgram {
    BloomProgram();
    ~BloomProgram();
    GLuint program;
    GLint color_texture;
    GLint bright_color_texture;
    GLint strength;
  };

  struct BlurProgram {
    BlurProgram();
    ~BlurProgram();
    GLuint program;
    GLint color_texture;
    GLint horizontal;
  };

  struct DepthProgram {
    DepthProgram();
    ~DepthProgram();
    GLuint program;
    GLint model_view_projection_matrix;
  };

  /** Uniforms for the standard shader. */
  class StandardProgram {
  public:
    StandardProgram();
    ~StandardProgram();
    GLuint program;
    GLint model_view_projection_matrix;
    GLint model_view_matrix;
    GLint model_matrix;
    GLint view_matrix;
    GLint normal_matrix;
    GLint depth_bias_mvp;

    GLint environment_map;
    GLint environment_position;
    GLint environment_extent;
    GLint environment_strength;

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

    GLint camera_resolution;
    GLint camera_position;

    GLint light_position;
    GLint light_color;
    GLint light_view;
    GLint light_projection;
    GLint light_shadow_map;
    GLint light_angle;
    GLint light_direction;

    GLint fog_color_near;
    GLint fog_color_far;
    GLint fog_attenuation_factor;

    GLint brdf_lut;
  };

  void render_texture_targets(const Scene &scene);

  void render_scene(const Camera &camera, const Scene &render_scene, const glm::vec2 &resolution);

  void render_shadow_map(const std::vector<Model> &models, const Light &light);

  void render_environment(const Scene &render_scene, const glm::vec4 &clear_color);

  void render_boxes(const Scene::Boxes & boxes, const mos::gfx::Camera &camera);
  void render_particles(const Scene::ParticleClouds &clouds, const mos::gfx::Camera &camera, const glm::vec2 &resolution);

  void render_model(const Model &model,
                    const glm::mat4 &transform,
                    const Camera &camera,
                    const Light &light,
                    const EnvironmentLight &environment,
                    const Fog &fog,
                    const glm::vec2 &resolution,
                    const StandardProgram& program);

  void render_model_depth(const Model &model,
                          const glm::mat4 &transform,
                          const Camera &camera,
                          const glm::vec2 &resolution,
                          const DepthProgram& program);

  /** Clear color and depth. */
  void clear(const glm::vec4 &color);

  unsigned int create_texture(const SharedTexture2D &texture);
  unsigned int create_texture(const Texture2D &texture);
  unsigned int create_texture_cube(const TextureCube &texture);

  static unsigned int create_shader(const std::string &source,
                             const unsigned int type, const std::string& name = "");
  static bool check_shader(const unsigned int shader, const std::string &name = "");
  static bool check_program(const unsigned int program, const std::string &name);
  static void link_program(const GLuint program,
                    const std::string& name);

  StandardProgram standard_program_;
  ParticleProgram particle_program_;
  BoxProgram box_program_;
  DepthProgram depth_program_;
  QuadProgram quad_program_;
  BloomProgram bloom_program_;
  BlurProgram blur_program_;

  std::unordered_map<unsigned int, GLuint> frame_buffers_;
  std::unordered_map<unsigned int, GLuint> render_buffers;
  std::unordered_map<unsigned int, GLuint> pixel_buffers_;
  std::unordered_map<unsigned int, Buffer> textures_;
  std::unordered_map<unsigned int, GLuint> texture_cubes_;
  std::unordered_map<unsigned int, Buffer> array_buffers_;
  std::unordered_map<unsigned int, Buffer> element_array_buffers_;
  std::unordered_map<unsigned int, GLuint> vertex_arrays_;
  std::unordered_map<unsigned int, PixelBuffer> test_buffers_;

  GLuint black_texture_;
  GLuint white_texture_;
  GLuint brdf_lut_texture_;
  GLuint box_vbo;
  GLuint box_ebo;
  GLuint box_va;

  struct FormatPair {
    GLuint internal_format;
    GLuint format;
  };
  //TODO: Static though, be aware initialization problem.
  std::map<Texture::Wrap, GLuint> wrap_map_;
  std::map<Texture::Format, FormatPair> format_map_;

  int cube_camera_index_;
};
}
}