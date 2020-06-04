#pragma once
#include <glad/glad.h>
#include <optional>
#include <initializer_list>
#include <unordered_map>
#include <array>
#include <future>
#include <glm/glm.hpp>
#include <mos/gfx/models.hpp>
#include <mos/gfx/scenes.hpp>

namespace mos::gfx {
  class Model;
  class Mesh;
  using Shared_mesh = std::shared_ptr<Mesh>;
  class Texture_2D;
  using Shared_texture_2D = std::shared_ptr<Texture_2D>;

/** Render geometry shapes with OpenGL. */
class Renderer final {
public:
  /** Inits the renderer, creates an OpenGL context with GLAD. */
  explicit Renderer(const glm::ivec2 &resolution, const int samples = 1);
  Renderer(const Renderer &renderer) = delete;
  Renderer(const Renderer &&renderer) = delete;
  Renderer & operator=(const Renderer & renderer) = delete;
  Renderer & operator=(const Renderer && renderer) = delete;
  ~Renderer() = default;

  /** Loads a model into GPU memory. */
  auto load(const Model &model) -> void;

  /** Load multiple models into GPU memory. */
  auto load(const Models & models) -> void;

  /** Unloads a model from GPU memory. */
  auto unload(const Model &model) -> void;

  /** Load a mesh into GPU memory. */
  auto load(const Mesh &mesh) -> void;

  /** Load a shared mesh into GPU memory.*/
  auto load(const Shared_mesh &mesh) -> void;

  /** Unloads a mesh from GPU memory. */
  auto unload(const Mesh &mesh) -> void;

  /** Unloads a shared mesh from GPU memory. */
  auto unload(const Shared_mesh &mesh) -> void;

  /** Loads a shared texture into GPU memory. */
  auto load(const Shared_texture_2D &texture) -> void;

  /** Loads a texture into GPU memory. */
  auto load_or_update(const Texture_2D &texture) -> void;

  /** Unloads a shared texture from GPU memory. */
  auto unload(const Shared_texture_2D &texture) -> void;

  /** Render multiple scenes. */
  auto render(const Scenes &scenes,
              const glm::vec4 &color = {0.0f, 0.0f, 0.0f, 1.0f},
              const glm::ivec2 &resolution = glm::ivec2(128, 128)) -> void;

  /** Clear all GPU buffers/memory. */
  auto clear_buffers() -> void;

private:
  using Time_point =  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

  static GLuint generate(const std::function<void(GLsizei, GLuint*)> & f);
  static GLuint wrap_convert(const Texture::Wrap& w);
  static GLuint filter_convert(const Texture::Filter &f);
  static GLuint filter_convert_mip(const Texture::Filter &f);

  struct FormatPair {
    GLint internal_format;
    GLenum format;
  };

  static FormatPair format_convert(const Texture::Format &f);

  #include <mos/gfx/renderer/buffer.hpp>
  #include <mos/gfx/renderer/render_buffers.hpp>
  #include <mos/gfx/renderer/vertex_arrays.hpp>
  #include <mos/gfx/renderer/texture_buffers.hpp>
  #include <mos/gfx/renderer/frame_buffers.hpp>
  #include <mos/gfx/renderer/array_buffers.hpp>
  #include <mos/gfx/renderer/element_array_buffers.hpp>

  #include <mos/gfx/renderer/light_uniforms.hpp> //TODO: remove

  #include <mos/gfx/renderer/cloud_program.hpp>
  #include <mos/gfx/renderer/box_program.hpp>
  #include <mos/gfx/renderer/bloom_program.hpp>
  #include <mos/gfx/renderer/compositing_program.hpp>
  #include <mos/gfx/renderer/blur_program.hpp>
  #include <mos/gfx/renderer/depth_program.hpp>
  #include <mos/gfx/renderer/propagate_program.hpp>
  #include <mos/gfx/renderer/environment_program.hpp>
  #include <mos/gfx/renderer/standard_program.hpp>

  #include <mos/gfx/renderer/standard_target.hpp>
  #include <mos/gfx/renderer/blit_target.hpp>
  #include <mos/gfx/renderer/post_target.hpp>
  #include <mos/gfx/renderer/shadow_map_target.hpp>
  #include <mos/gfx/renderer/environment_map_target.hpp>

  #include <mos/gfx/renderer/box.hpp>
  #include <mos/gfx/renderer/quad.hpp>

  auto render_texture_targets(const Scene &scene) -> void;

  auto render_scene(const Camera &camera,
                    const Scene &scene,
                    const glm::ivec2 &resolution) -> void;

  auto render_shadow_maps(const Models &models,
                          const Lights &lights) -> void;

  auto render_environment(const Scene &scene,
                          const glm::vec4 &clear_color) -> void;

  auto render_boxes(const Boxes & boxes,
                    const mos::gfx::Camera &camera) -> void;

  auto render_sky(const Model &model,
                    const Camera &camera,
                    const Lights &lights,
                    const Environment_lights &environment_lights,
                    const Fog &fog,
                    const glm::vec2 &resolution,
                    const Standard_program& program) -> void;

  auto render_clouds(const Clouds &clouds,
                    const Lights &lights,
                    const Environment_lights &environment_lights,
                    const mos::gfx::Camera &camera,
                    const glm::ivec2 &resolution,
                    const Cloud_program &program,
                    const GLenum &draw_mode) -> void;

  auto render_model(const Model &model,
                    const glm::mat4 &transform,
                    const Camera &camera,
                    const Lights &lights,
                    const Environment_lights &environment_lights,
                    const Fog &fog,
                    const glm::vec2 &resolution,
                    const Standard_program& program) -> void;

  auto render_model(const Model &model,
                    const glm::mat4 &transform,
                    const Camera &camera,
                    const Lights &lights,
                    const Environment_lights &environment_lights,
                    const Fog &fog,
                    const glm::vec2 &resolution,
                    const Environment_program& program) -> void;

  auto render_model_depth(const Model &model,
                          const glm::mat4 &transform,
                          const Camera &camera,
                          const glm::vec2 &resolution,
                          const Depth_program& program) -> void;

  /** Clear color and depth. */
  auto clear(const glm::vec4 &color) -> void;
  auto clear_depth() -> void;
  auto clear_color(const glm::vec4 &color) -> void;
  auto blur(GLuint input_texture,
            const Post_target &buffer_target,
            const Post_target &output_target,
            float iterations = 6) -> void;

  const bool context_;

  const Shader functions_shader_;
  const Propagate_program propagate_program_;
  const Standard_program standard_program_;
  const Environment_program environment_program_;
  const Cloud_program point_cloud_program_;
  const Cloud_program line_cloud_program_;
  const Box_program box_program_;
  const Depth_program depth_program_;
  const Bloom_program bloom_program_;
  const Compositing_program compositing_program_;
  const Blur_program blur_program_;

  Frame_buffers frame_buffers_;
  Render_buffers render_buffers_;
  Texture_buffers textures_;
  Array_buffers array_buffers_;
  Element_array_buffers element_array_buffers_;
  Vertex_arrays vertex_arrays_;

  const Standard_target standard_target_;
  const Blit_target multisample_target_;
  const Post_target post_target0_;
  const Post_target post_target1_;

  const Quad quad_;
  const Box box;

  const Texture_buffer_2D black_texture_;
  const Texture_buffer_2D white_texture_;
  const Texture_buffer_2D brdf_lut_texture_;

  std::array<int,2> cube_camera_index_;

  /** Shadow maps. */
  const Render_buffer shadow_maps_render_buffer_;
  const std::array<Shadow_map_target, 4> shadow_maps_;
  const Post_target shadow_map_blur_target_;
  const std::array<Post_target, 4> shadow_map_blur_targets_;

  /** Environment map targets. */
  const Render_buffer environment_render_buffer_;
  const std::array<Environment_map_target, 2> environment_maps_targets_;
  const Environment_map_target propagate_target_;
};
}

