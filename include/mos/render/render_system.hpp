#ifndef MOS_RENDERER_H
#define MOS_RENDERER_H

#include <GL/glew.h>
#include <optional.hpp>
#include <initializer_list>
#include <unordered_map>
#include <optional.hpp>
#include <array>
#include <mos/render/models_batch.hpp>
#include <mos/render/texture.hpp>
#include <mos/render/model.hpp>
#include <mos/render/quad.hpp>
#include <mos/render/text.hpp>
#include <mos/render/particles.hpp>
#include <mos/render/light.hpp>
#include <mos/render/render_target.hpp>
#include <mos/render/camera.hpp>
#include <mos/render/fog_exp.hpp>
#include <mos/render/fog_linear.hpp>
#include <mos/render/render_types.hpp>
#include <mos/render/particles_batch.hpp>
#include <mos/render/boxes_batch.hpp>
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
  void load(const std::shared_ptr<Texture> &texture);

  /**
   * @brief unload a texture from renderer memory.
   * @param texture The texture.
   */
  void unload(const std::shared_ptr<Texture> &texture);



  void batches(const std::initializer_list<ModelsBatch> &batches_init,
               const std::initializer_list<ParticlesBatch> &particles_batches,
               const std::initializer_list<BoxesBatch> &boxes_batches,
               const glm::vec4 &color = glm::vec4(.0f), const OptTarget &target = OptTarget());

  template <class Tr, class Tp, class Tb>
  void batches(Tr begin, Tr end,
               Tp p_begin, Tp p_end,
               Tb b_begin, Tb b_end, const glm::vec4 &color = {.0f, .0f, .0f, 1.0f},
               const OptTarget &target = OptTarget()) {
    render_target(target);
    clear(color);
    for (auto it = begin; it != end; it++) {
      models_batch(*it);
    }

    for (auto it = p_begin; it != p_end; it++){
      particles_batch(*it);
    }

    for (auto it = b_begin; it != b_end; it++){
      boxes_batch(*it);
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
   * @brief particles_batch rendering.
   * @param batch
   */
  void particles_batch(const ParticlesBatch &batch);

  /**
   * @brief boxes_batch rendering.
   * @param batch
   */
  void boxes_batch(const BoxesBatch &batch);

  /**
   * @brief models_batch rendering.
   * @param batch
   */
  void models_batch(const ModelsBatch &batch);

  /**
   * @brief Updates render state of model.
   * @param model
   * @param transform Additional transform matrix.
   * @param view View matrix of the camera
   * @param projection Projection matrix of the camera
   * @param program_name Either "text" or "standard"
   * @param light One dynamic light to use.
   */
  void models(const Model &model, const glm::mat4 transform,
              const glm::mat4 view, const glm::mat4 projection,
              const glm::vec2 &resolution = glm::vec2(0.0f),
              const Light &light = Light(),
              const FogExp &fog_exp = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),
              const FogLinear &fog_linear = FogLinear(),
              const glm::vec3 &multiply = glm::vec3(1.0f),
              const ModelsBatch::Shader &shader = ModelsBatch::Shader::STANDARD,
              const ModelsBatch::Draw &draw = ModelsBatch::Draw::TRIANGLES);


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
  struct VertexProgramData {
    GLuint program;
    GLint mvp;
    GLint mv;
    GLint normal_matrix;
    GLint depth_bias_mvp;
    GLint texture;
    GLint lightmap;
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

  using VertexProgramPair = std::pair<ModelsBatch::Shader, VertexProgramData>;
  using ParticleProgramPair = std::pair<std::string, ParticleProgramData>;
  using BoxProgramPair = std::pair<std::string, BoxProgramData>;

  bool lightmaps_;
  void add_vertex_program(const ModelsBatch::Shader shader,
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

  std::map<ModelsBatch::Shader, VertexProgramData> vertex_programs_;
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
