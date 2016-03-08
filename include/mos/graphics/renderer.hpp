#ifndef MOS_RENDERER_H
#define MOS_RENDERER_H

#include "GL/glew.h"
#include <mos/graphics/texture2d.hpp>
#include <mos/graphics/model.hpp>
#include <mos/graphics/quad.hpp>
#include <mos/graphics/text.hpp>
#include <mos/graphics/particles.hpp>
#include <mos/graphics/light.hpp>
#include <mos/graphics/target.hpp>
#include <mos/graphics/camera.hpp>
#include <mos/graphics/fog.hpp>
#include <mos/simulation/box.hpp>
#include <optional.hpp>

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
  using OptClear = std::experimental::optional<glm::vec3>;
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
  void load(const std::shared_ptr<Texture2D> &texture);

  /**
   * @brief unload a texture from renderer memory.
   * @param texture The texture.
   */
  void unload(const std::shared_ptr<Texture2D> &texture);

  /**
   * @brief render_target
   * @param target
   */
  void render_target(const OptTarget & target);

  /**
   * @brief clear
   * @param color
   */
  void clear(const glm::vec4 & color);

  /**
   * @brief Updates render state of model.
   * @param model
   * @param transform Additional transform matrix.
   * @param view View matrix of the camera
   * @param projection Projection matrix of the camera
   * @param program_name Either "text" or "standard"
   * @param light One dynamic light to use.
   */
  void update(const Model &model, const glm::mat4 transform,
              const glm::mat4 view, const glm::mat4 projection,
              const glm::vec2 &resolution = glm::vec2(0.0f),
              const Light &light = Light(),
              const Fog &fog = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

  /**
   * @brief update
   * @param model
   * @param view
   * @param projection
   * @param light
   * @param resolution
   * @param target
   */
  void update(const Model &model,
              const glm::mat4 &view,
              const glm::mat4 &projection,
              const glm::vec2 &resolution,
              const Light &light = Light(),
              const Fog &fog = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

  /**
   * @brief update
   * @param model
   * @param camera
   * @param light
   * @param resolution
   * @param target
   */
  void update(const Model &model,
              const Camera &camera,
              const glm::vec2 &resolution,
              const Light &light = Light(),
              const Fog &fog = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

  /**
   * @brief Renders particles.
   * @param particles Particles object.
   * @param view View matrix.
   * @param projection Projection matrix.
   */
  void update(Particles &particles, const glm::mat4 view,
              const glm::mat4 projection);

  void update(const Box &box, const glm::mat4 &view,
              const glm::mat4 &projection);

  void update(const Box &box, const Camera &camera);

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

private:
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

  /**
   * @brief The VertexProgramData struct, uniforms for the standard shader.
   */
  struct VertexProgramData {
    GLuint program;
    GLint mvp;
    GLint mv;
    GLint normal_matrix;
    GLint texture;
    GLint texture2;
    GLint lightmap;
    GLint normalmap;
    GLint material_ambient_color;
    GLint material_diffuse_color;
    GLint material_specular_color;
    GLint material_specular_exponent;
    GLint opacity;
    GLint light_position;
    GLint light_diffuse_color;
    GLint light_specular_color;
    GLint has_texture;
    GLint has_texture2;
    GLint has_lightmap;
    GLint has_normalmap;
    GLint has_material;
    GLint receives_light;
    GLint resolution;
    GLint fog_color;
    GLint fog_density;
  };

  using VertexProgramPair = std::pair<Model::Shader, VertexProgramData>;
  using ParticleProgramPair = std::pair<std::string, ParticleProgramData>;
  using BoxProgramPair = std::pair<std::string, BoxProgramData>;

  bool lightmaps_;
  void add_vertex_program(const Model::Shader shader,
                          const std::string vertex_shader_source,
                          const std::string fragment_shader_source);

  void add_particle_program(const std::string name, const std::string vs_source,
                            const std::string fs_source);

  unsigned int create_shader(const std::string source, const unsigned int type);
  bool check_shader(const unsigned int shader);
  bool check_program(const unsigned int program);
  unsigned int create_texture(std::shared_ptr<mos::Texture2D> texture);
  unsigned int
  create_texture_and_pbo(const std::shared_ptr<mos::Texture2D> &texture);
  void add_box_program(const std::string &name, const std::string &vs_source,
                       const std::string &fs_source);

  std::map<Model::Shader, VertexProgramData> vertex_programs_;
  std::unordered_map<std::string, ParticleProgramData> particle_programs_;
  std::unordered_map<std::string, BoxProgramData> box_programs_;

  std::unordered_map<unsigned int, GLuint> frame_buffers_;
  std::unordered_map<unsigned int, GLuint> render_buffers;
  std::unordered_map<unsigned int, GLuint> pixel_buffers_;
  std::unordered_map<unsigned int, GLuint> textures_;
  std::unordered_map<unsigned int, GLuint> array_buffers_;
  std::unordered_map<unsigned int, GLuint> element_array_buffers_;
  std::unordered_map<unsigned int, GLuint> vertex_arrays_;
  unsigned int box_vbo;
  unsigned int box_ebo;
  unsigned int box_va;
};
}
#endif /* MOS_RENDERER_H */
