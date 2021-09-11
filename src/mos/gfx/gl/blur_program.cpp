#include <mos/gfx/gl/renderer.hpp>
#include <mos/util.hpp>

namespace mos::gfx::gl {
Renderer::Blur_program::Blur_program() {
  std::string name = "blur";
  auto vert_source = text("assets/shaders/" + name + ".vert");
  auto frag_source = text("assets/shaders/" + name + ".frag");

  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);

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
