#include <iostream>
#include <mos/gl/renderer.hpp>
#include <spdlog/spdlog.h>

namespace mos::gfx::gl {

Renderer::Shader::Shader(const std::string &source, const GLuint type,
                         const std::string &name)
    : id(glCreateShader(type)) {

  static const std::map<const unsigned int, std::string> shader_types{
      {GL_VERTEX_SHADER, "vertex shader"},
      {GL_FRAGMENT_SHADER, "fragment shader"},
      {GL_GEOMETRY_SHADER, "geometry shader"}};

  auto const *chars = source.c_str();
  spdlog::info("Compiling: {} {}", name, shader_types.at(type));
  glShaderSource(id, 1, &chars, nullptr);
  glCompileShader(id);

  assert(id);

  GLint status{0};
  glGetShaderiv(id, GL_COMPILE_STATUS, &status);

  if (status == GL_FALSE) {
    int length{0};
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    std::vector<char> buffer(length);
    glGetShaderInfoLog(id, length, nullptr, &buffer[0]);
    spdlog::error("Compile failure in: {} {} \n {}", name, shader_types.at(type), std::string(buffer.begin(), buffer.end()));
  }
  assert(status);
}
Renderer::Shader::~Shader() { glDeleteShader(id); }

} // namespace mos::gfx
