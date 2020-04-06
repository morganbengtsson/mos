#include <mos/gfx/renderer.hpp>
#include <mos/util.hpp>

namespace mos::gfx {
Renderer::Depth_program::Depth_program() {
  std::string name = "depth";
  auto vert_source = text("assets/shaders/" + name + ".vert");
  auto frag_source = text("assets/shaders/" + name + ".frag");

  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);

  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, fragment_shader.id);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 3, "uv");
  link(name);
  check(name);
  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);

  model_view_projection = glGetUniformLocation(program, "model_view_projection");
  albedo_sampler = glGetUniformLocation(program, "albedo_sampler");
  albedo = glGetUniformLocation(program, "albedo");
  emission = glGetUniformLocation(program, "emission");
}
}
