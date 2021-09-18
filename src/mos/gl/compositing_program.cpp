#include <mos/gl/renderer.hpp>
#include <mos/util.hpp>

namespace mos::gl {
Compositing_program::Compositing_program() {
  std::string defines = "#version 430 core\n";

#ifdef MOS_EFFECT_VIGNETTE
  defines += "#define VIGNETTE\n";
#endif
#ifdef MOS_EFFECT_VHS
  defines += "#define VHS\n";
#endif
#ifdef MOS_EFFECT_DITHER
  defines += "#define DITHER\n";
#endif

  std::string name = "compositing";
  auto vert_source = text("assets/shaders/" + name + ".vert");
  auto frag_source = text("assets/shaders/" + name + ".frag");

  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(defines + frag_source, GL_FRAGMENT_SHADER, name);

  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, fragment_shader.id);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "uv");
  link(name);
  check(name);

  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);

  color_sampler = glGetUniformLocation(program, "color_sampler");
  bloom_sampler = glGetUniformLocation(program, "bloom_sampler");
  bloom_strength = glGetUniformLocation(program, "strength");
}
} // namespace mos::gfx
