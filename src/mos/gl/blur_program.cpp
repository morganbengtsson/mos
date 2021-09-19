#include <mos/gl/renderer.hpp>
#include <mos/util.hpp>
#include <spdlog/fmt/fmt.h>

namespace mos::gl {
Blur_program::Blur_program() {
  const std::string name = "blur";

  const auto vertex_shader = Shader(name, GL_VERTEX_SHADER);
  const auto fragment_shader = Shader(name, GL_FRAGMENT_SHADER);

  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, fragment_shader.id);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "uv");
  link(name);
  check(name);

  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);
  color_sampler = glGetUniformLocation(program, "color_sampler");
  horizontal = glGetUniformLocation(program, "horizontal");
}
} // namespace mos::gfx
