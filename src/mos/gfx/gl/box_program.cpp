#include <mos/gfx/gl/renderer.hpp>
#include <mos/util.hpp>

namespace mos::gfx::gl {
Renderer::Box_program::Box_program() {
  std::string name = "box";
  std::string vert_source = text("assets/shaders/" + name + ".vert");
  std::string frag_source = text("assets/shaders/" + name + ".frag");

  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);

  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, fragment_shader.id);
  glBindAttribLocation(program, 0, "position");

  link(name);
  check(name);

  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);

  model_view_projection =
      glGetUniformLocation(program, "model_view_projection");
}
} // namespace mos::gfx
