#include <mos/gl/renderer.hpp>
#include <mos/util.hpp>

namespace mos::gl {

Depth_program::Depth_program() {
  const std::string name = "depth";

  const auto vertex_shader = Shader(name, GL_VERTEX_SHADER);
  const auto fragment_shader = Shader(name, GL_FRAGMENT_SHADER);

  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, fragment_shader.id);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 3, "uv");
  link(name);
  check(name);
  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);

  model_view_projection =
      glGetUniformLocation(program, "model_view_projection");
  albedo_sampler = glGetUniformLocation(program, "albedo_sampler");
  albedo = glGetUniformLocation(program, "albedo");
  emission = glGetUniformLocation(program, "emission");
}
} // namespace mos::gfx
