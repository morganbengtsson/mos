#include <mos/gfx/renderer.hpp>
#include <mos/util.hpp>

namespace mos::gfx {
Renderer::Propagate_program::Propagate_program() {
  std::string name = "propagate";
  auto vert_source = text("assets/shaders/" + name + ".vert");
  auto frag_source = text("assets/shaders/" + name + ".frag");

  std::string functions_name = "functions";
  std::string functions_frag_source = text("assets/shaders/" + functions_name + ".frag");

  const auto functions_fragment_shader = Shader(functions_frag_source, GL_FRAGMENT_SHADER, functions_name);
  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);

  glAttachShader(program, functions_fragment_shader.id);
  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, fragment_shader.id);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "uv");
  link(name);
  check(name);

  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);
  glDetachShader(program, functions_fragment_shader.id);

  environment_sampler = glGetUniformLocation(program, "environment_map");
  environment_albedo_sampler = glGetUniformLocation(program, "environment_albedo_map");
  side = glGetUniformLocation(program, "side");
}
}
