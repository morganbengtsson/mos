#ifndef MOS_RENDERER_H
#define MOS_RENDERER_H

#include <GL/glew.h>
#include <optional.hpp>
#include <initializer_list>
#include <unordered_map>
#include <optional.hpp>
#include <array>
#include <mos/render/render_scene.hpp>
#include <mos/render/texture.hpp>
#include <mos/render/texture_cube.hpp>
#include <mos/render/model.hpp>
#include <mos/render/quad.hpp>
#include <mos/render/text.hpp>
#include <mos/render/particles.hpp>
#include <mos/render/light.hpp>
#include <mos/render/render_target.hpp>
#include <mos/render/render_camera.hpp>
#include <mos/render/fog_linear.hpp>
#include <mos/render/render_box.hpp>

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

  // Temporary method
  void update_depth(const Model &model, const glm::mat4 &parent_transform,
                    const glm::mat4 &view, const glm::mat4 &projection,
                    const glm::vec2 &resolution);

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
  void load(const SharedTexture &texture);

  /**
   * @brief unload a texture from renderer memory.
   * @param texture The texture.
   */
  void unload(const SharedTexture &texture);

  /**
   * @brief Load a texture into renderer memory.
   * @param texture The texture.
   */
  void load(const SharedTextureCube &texture);

  /**
   * @brief unload a texture from renderer memory.
   * @param texture The texture.
   */
  void unload(const SharedTextureCube &texture);

  void batches(const std::initializer_list<RenderScene> &scenes_init,
               const glm::vec4 &color = glm::vec4(.0f), const OptTarget &target = OptTarget());

  template<class Tr>
  void batches(Tr begin, Tr end, const glm::vec4 &color = {.0f, .0f, .0f, 1.0f},
               const OptTarget &target = OptTarget()) {
    render_target(target);
    clear(color);
    for (auto it = begin; it != end; it++) {
      scene(*it);
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

  /**
   * @brief Set lightmap use.
   */
  void lightmaps(const bool lightmaps);

  /**
   * @brief Check if lightmaps is enabled.
   * @return True if lightmaps are enabled.
   */
  bool lightmaps() const;

  // TODO: Private
  GLuint depth_texture_;
  GLuint depth_frame_buffer_;

private:
  /**
   * @brief models_batch rendering.
   * @param render_scene
   */
  void scene(const RenderScene &render_scene);

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
              const glm::mat4 transform,
              const RenderCamera & camera,
              const Light &light,
              const FogLinear &fog_linear,
              const glm::vec3 &multiply,
              const RenderScene::Shader &shader,
              const RenderScene::Draw &draw);

  /**
   * @brief render_target
   * @param target
   */
  void render_target(const OptTarget &target);

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
    VertexProgramData(){};
    VertexProgramData(const GLuint program);
    GLuint program;
    GLint model_view_projection_matrix;
    GLint model_view_matrix;
    GLint model_matrix;
    GLint view_matrix;
    GLint normal_matrix;
    GLint depth_bias_mvp;
    GLint diffuse_map;
    GLint light_map;
    GLint normal_map;
    GLint shadow_map;
    GLint diffuse_environment_map;
    GLint specular_environment_map;
    GLint material_ambient_color;
    GLint material_diffuse_color;
    GLint material_specular_color;
    GLint material_specular_exponent;
    GLint opacity;
    GLint camera_position;
    GLint light_position;
    GLint light_diffuse_color;
    GLint light_specular_color;
    GLint light_ambient_color;
    GLint light_view;
    GLint light_projection;
    GLint receives_light;
    GLint resolution;
    GLint fogs_linear_color;
    GLint fogs_linear_near;
    GLint fogs_linear_far;
    GLint time;
    GLint overlay;
    GLint multiply;
  };

  using VertexProgramPair = std::pair<RenderScene::Shader, VertexProgramData>;
  using ParticleProgramPair = std::pair<std::string, ParticleProgramData>;
  using BoxProgramPair = std::pair<std::string, BoxProgramData>;

  bool lightmaps_;
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

  unsigned int create_texture(const SharedTexture &texture);

  unsigned int create_texture_cube(const SharedTextureCube &texture);

  unsigned int
  create_texture_and_pbo(const SharedTexture &texture);
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

  GLuint empty_texture_;
  GLuint box_vbo;
  GLuint box_ebo;
  GLuint box_va;
};
}
#endif /* MOS_RENDERER_H */
