#ifndef MOS_RENDERER_H
#define MOS_RENDERER_H

#include "GL/glew.h"
#include <mos/graphics/batch.hpp>
#include <mos/graphics/texture.hpp>
#include <mos/graphics/model.hpp>
#include <mos/graphics/quad.hpp>
#include <mos/graphics/text.hpp>
#include <mos/graphics/particles.hpp>
#include <mos/graphics/light.hpp>
#include <mos/graphics/target.hpp>
#include <mos/graphics/camera.hpp>
#include <mos/graphics/fog_exp.hpp>
#include <mos/simulation/box.hpp>
#include <optional.hpp>
#include <initializer_list>
#include <unordered_map>
#include <optional.hpp>
#include <array>

namespace mos {

/**
 * @brief Render geometry.
 * The class that talks to OpenGL, and renders Model objects.
 */
class Renderer {
public:
  using OptTarget = std::experimental::optional<Target>;
  /**
   * @brief Renderer constructor.
   * Inits the renderer, in this implementation also creates a
   * valid OpenGL context with GLEW.
   */
  Renderer();
  /**
   * @brief Renderer destructor.
   * Deletes all allocated GPU memory. Textures, Shaders, Buffers.
   */
  virtual ~Renderer();

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
  void load(const std::shared_ptr<Texture> &texture);

  /**
   * @brief unload a texture from renderer memory.
   * @param texture The texture.
   */
  void unload(const std::shared_ptr<Texture> &texture);

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

  void batches(const std::initializer_list<Batch> &batches_init,
               const glm::vec4 &color, const OptTarget &target = OptTarget());

  template <class T>
  void batches(T begin, T end, const glm::vec4 &color = {.0f, .0f, .0f, 1.0f},
               const OptTarget &target = OptTarget()) {
    render_target(target);
    clear(color);
    for (auto it = begin; it != end; it++) {
      glUseProgram(vertex_programs_[it->shader].program);
      for (auto &m : it->models) {
        update(m, it->view, it->projection, 0.0f, it->resolution, it->light,
               it->fog, 1.0f, it->shader);
      }
    }
  }

  /**
   * @brief Updates render state of model.
   * @param model
   * @param transform Additional transform matrix.
   * @param view View matrix of the camera
   * @param projection Projection matrix of the camera
   * @param program_name Either "text" or "standard"
   * @param light One dynamic light to use.
   */
  [[deprecated("To be removed/renamed in favor of batches(...)/batch(...)")]]
  void update(const Model &model, const glm::mat4 transform,
              const glm::mat4 view, const glm::mat4 projection, const float dt,
              const glm::vec2 &resolution = glm::vec2(0.0f),
              const Light &light = Light(),
              const FogExp &fog = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),
              const float multiply = 1.0f,
              const Batch::Shader &shader = Batch::Shader::STANDARD,
              const Batch::Draw &draw = Batch::Draw::TRIANGLES);

  /**
   * @brief update
   * @param model
   * @param view
   * @param projection
   * @param light
   * @param resolution
   * @param target
   */
  [[deprecated("To be removed/renamed in favor of batches(...)/batch(...)")]]
  void update(const Model &model, const glm::mat4 &view,
              const glm::mat4 &projection, const float dt,
              const glm::vec2 &resolution, const Light &light = Light(),
              const FogExp &fog = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),
              const float multiply = 1.0f,
              const Batch::Shader &shader = Batch::Shader::STANDARD,
              const Batch::Draw &draw = Batch::Draw::TRIANGLES);

  /**
   * @brief update
   * @param model
   * @param camera
   * @param light
   * @param resolution
   * @param target
   */
  [[deprecated("To be removed/renamed in favor of batches(...)/batch(...)")]]
  void update(const Model &model, const Camera &camera, const float dt,
              const glm::vec2 &resolution, const Light &light = Light(),
              const FogExp &fog = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),
              const float multiply = 1.0f);

  /**
   * @brief Renders particles.
   * @param particles Particles object.
   * @param view View matrix.
   * @param projection Projection matrix.
   */
  void update(Particles &particles, const glm::mat4 view,
              const glm::mat4 projection, const float dt);

  void update(const Box &box, const glm::mat4 &view,
              const glm::mat4 &projection, const float dt);

  void update(const Box &box, const Camera &camera, const float dt);

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
   * @brief Amount of time the renderer has been alive, used in shaders.
   */
  float time_;
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
  struct VertexProgramData {
    GLuint program;
    GLint mvp;
    GLint mv;
    GLint normal_matrix;
    GLint depth_bias_mvp;
    GLint textures_first;
    GLint textures_second;
    GLint lightmaps_first;
    GLint lightmaps_second;
    GLint lightmaps_mix;
    GLint normalmap;
    GLint shadowmap;
    GLint material_ambient_color;
    GLint material_diffuse_color;
    GLint material_specular_color;
    GLint material_specular_exponent;
    GLint opacity;
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
    GLint fogs_exp_color;
    GLint fogs_exp_density;
    GLint time;
    GLint overlay;
    GLint multiply;
  };

  using VertexProgramPair = std::pair<Batch::Shader, VertexProgramData>;
  using ParticleProgramPair = std::pair<std::string, ParticleProgramData>;
  using BoxProgramPair = std::pair<std::string, BoxProgramData>;

  bool lightmaps_;
  void add_vertex_program(const Batch::Shader shader,
                          const std::string vertex_shader_source,
                          const std::string fragment_shader_source);

  void add_particle_program(const std::string name, const std::string vs_source,
                            const std::string fs_source);

  unsigned int create_shader(const std::string &source,
                             const unsigned int type);
  bool check_shader(const unsigned int shader);
  bool check_program(const unsigned int program);
  unsigned int create_texture(std::shared_ptr<mos::Texture> texture);
  unsigned int
  create_texture_and_pbo(const std::shared_ptr<mos::Texture> &texture);
  void add_box_program(const std::string &name, const std::string &vs_source,
                       const std::string &fs_source);

  void create_depth_program();

  std::map<Batch::Shader, VertexProgramData> vertex_programs_;
  std::unordered_map<std::string, ParticleProgramData> particle_programs_;
  std::unordered_map<std::string, BoxProgramData> box_programs_;
  DepthProgramData depth_program_;

  std::unordered_map<unsigned int, GLuint> frame_buffers_;
  std::unordered_map<unsigned int, GLuint> render_buffers;
  std::unordered_map<unsigned int, GLuint> pixel_buffers_;
  std::unordered_map<unsigned int, GLuint> textures_;
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
