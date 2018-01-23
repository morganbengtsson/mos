#ifndef MOS_RENDERER_H
#define MOS_RENDERER_H

#include <GL/glew.h>
#include <experimental/optional>
#include <initializer_list>
#include <unordered_map>
#include <array>
#include <mos/render/render_scene.hpp>
#include <mos/render/texture_2d.hpp>
#include <mos/render/texture_cube.hpp>
#include <mos/render/model.hpp>
#include <mos/render/text.hpp>
#include <mos/render/particles.hpp>
#include <mos/render/light.hpp>
#include <mos/render/render_target.hpp>
#include <mos/render/render_camera.hpp>
#include <mos/render/environment_light.hpp>
#include <mos/render/fog.hpp>
#include <mos/render/render_box.hpp>
#include <mos/render/decal.hpp>
#include "texture.hpp"

namespace mos {

/**
 * @brief Render geometry.
 * Talks to OpenGL, and renders Model objects.
 */
class RenderSystem {
public:
  /**
   * @brief Renderer constructor.
   * Inits the renderer, in this implementation also creates a
   * valid OpenGL context with GLEW.
   */
  RenderSystem(const glm::vec4 &color = glm::vec4(.0f));
  /**
   * @brief Renderer destructor.
   * Deletes all allocated GPU memory. Textures, Shaders, Buffers.
   */
  ~RenderSystem();

  /**
   * @brief load a model into renderers own memory.
   * @param model
   */
  void load(const Model &model);

  /**
   * @brief unload a model from renderers own memory.
   * @param model
   */
  void unload(const Model &model);

  /**
   * @brief Load a texture into renderer memory.
   * @param texture The texture.
   */
  void load(const SharedTexture2D &texture);

  void load(const Texture2D &texture);

  /**
   * @brief unload a texture from renderer memory.
   * @param texture The texture.
   */
  void unload(const SharedTexture2D &texture);

  /**
   * @brief unload a texture from renderer memory.
   * @param texture The texture.
   */
  void unload(const SharedTextureCube &texture);

  void render_scenes(const std::initializer_list<RenderScene> &scenes_init,
                     const glm::vec4 &color = glm::vec4(.0f),
                     const glm::ivec2 &resolution = glm::ivec2(128, 128));


  template<class Ts>
  void render_scenes(Ts scenes_begin,
                     Ts scenes_end,
                     const glm::vec4 &color = {.0f, .0f, .0f, 1.0f},
                     const glm::ivec2 &resolution = glm::ivec2(128, 128)) {
    clear(color);
    for (auto it = scenes_begin; it != scenes_end; it++) {
      render_shadow_map(*it);
      render_environment(*it);
      render_scene(it->camera, *it, resolution);
    }
  }

  /**
   * @brief Clear all internal buffers.
   */
  void clear_buffers();

  /*
   * For multisampled render target
  void render_target_reset(unsigned int width, unsigned int height) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  */

private:
   /**
   * @brief models_batch rendering.
   * @param render_scene
   */
  void render_scene(const RenderCamera &camera, const RenderScene &render_scene, const glm::vec2 &resolution);

  void render_shadow_map(const RenderScene &render_scene);

  void render_environment(const RenderScene &render_scene);

  /**
   * @brief Updates render state of model.
   * @param model
   * @param transform Additional transform matrix.
   * @param view View matrix of the camera
   * @param projection Projection matrix of the camera
   * @param program_name Either "text" or "standard"
   * @param light One dynamic light to use.
   */
  void render(const Model &model,
              const RenderScene::Decals &decals,
              const glm::mat4 &transform,
              const RenderCamera &camera,
              const Light &light,
              const EnvironmentLight &environment,
              const Fog &fog,
              const glm::vec2 &resolution,
              const RenderScene::Shader &shader,
              const RenderScene::Draw &draw);

  /**
   * @brief render_target
   * @param target
   */
  void render_target(const RenderTarget &target);

  /**
   * @brief clear
   * @param color
   */
  void clear(const glm::vec4 &color);

  /**
   * @brief The ParticleProgramData struct, uniforms for the particle shader
   * program.
   */
  struct ParticleProgramData {
    GLuint program;
    GLint mvp;
    GLint mv;
    GLint p;
    GLint texture;
    GLint resolution;
  };

  /**
   * @brief The BoxProgramData struct, uniforms for the bounding box shader
   * program.
   */
  struct BoxProgramData {
    GLuint program;
    GLint mvp;
    GLint mv;
  };

  struct DepthProgramData {
    GLuint program;
    GLint mvp;
  };

  /**
   * @brief The VertexProgramData struct, uniforms for the standard shader.
   */
  class VertexProgramData {
  public:
    //TODO make all const
    VertexProgramData() {};
    VertexProgramData(const GLuint program);
    GLuint program;
    GLint model_view_projection_matrix;
    GLint model_view_matrix;
    GLint model_matrix;
    GLint view_matrix;
    GLint normal_matrix;
    GLint depth_bias_mvp;

    std::array<GLint, 10> decal_mvps;
    std::array<GLint, 10> decal_material_diffuse_maps;
    std::array<GLint, 10> decal_material_normal_maps;

    GLint environment_map;
    GLint environment_position;
    GLint environment_extent;
    GLint environment_strength;

    GLint material_albedo_map;
    GLint material_light_map;
    GLint material_normal_map;
    GLint material_metallic_map;
    GLint material_roughness_map;
    GLint material_ambient_occlusion_map;
    GLint material_albedo;
    GLint material_roughness;
    GLint material_metallic;
    GLint material_opacity;
    GLint material_emission;

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

  using VertexProgramPair = std::pair<RenderScene::Shader, VertexProgramData>;
  using ParticleProgramPair = std::pair<std::string, ParticleProgramData>;
  using BoxProgramPair = std::pair<std::string, BoxProgramData>;

  void add_vertex_program(const RenderScene::Shader shader,
                          const std::string vertex_shader_source,
                          const std::string fragment_shader_source,
                          const std::string &vert_file_name = "",
                          const std::string &frag_file_name = "");

  void add_particle_program(const std::string name, const std::string vs_source,
                            const std::string fs_source, const std::string &vs_file = "",
                            const std::string &fs_file = "");

  unsigned int create_shader(const std::string &source,
                             const unsigned int type);
  bool check_shader(const unsigned int shader, const std::string &name = "");

  bool check_program(const unsigned int program);

  unsigned int create_texture(const SharedTexture2D &texture);
  unsigned int create_texture(const Texture2D &texture);

  unsigned int create_texture_cube(const TextureCube &texture);

  unsigned int
  create_texture_and_pbo(const SharedTexture2D &texture);
  void add_box_program(const std::string &name, const std::string &vs_source,
                       const std::string &fs_source, const std::string &vs_file,
                       const std::string &fs_file);

  void create_depth_program();

  std::map<RenderScene::Shader, VertexProgramData> vertex_programs_;
  std::unordered_map<std::string, ParticleProgramData> particle_programs_;
  std::unordered_map<std::string, BoxProgramData> box_programs_;
  DepthProgramData depth_program_;

  std::unordered_map<unsigned int, GLuint> frame_buffers_;
  std::unordered_map<unsigned int, GLuint> render_buffers;
  std::unordered_map<unsigned int, GLuint> pixel_buffers_;
  std::unordered_map<unsigned int, GLuint> textures_;
  std::unordered_map<unsigned int, GLuint> texture_cubes_;
  std::unordered_map<unsigned int, GLuint> array_buffers_;
  std::unordered_map<unsigned int, GLuint> element_array_buffers_;
  std::unordered_map<unsigned int, GLuint> vertex_arrays_;

  GLuint black_texture_;
  GLuint white_texture_;
  GLuint brdf_lut_texture_;
  GLuint box_vbo;
  GLuint box_ebo;
  GLuint box_va;

  struct FormatPair{
    GLuint internal_format;
    GLuint format;
  };
  //TODO: Static though, be aware initialization problem.
  std::map<Texture::Wrap, GLuint> wrap_map_;
  std::map<Texture::Format, FormatPair> format_map_;
  std::map<RenderScene::Draw, GLuint> draw_map_;
};

}
#endif /* MOS_RENDERER_H */
