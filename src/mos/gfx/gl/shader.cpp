#include <iostream>
#include <mos/gfx/gl/renderer.hpp>

namespace mos::gfx {

Renderer::Shader::Shader(const std::string &source, const GLuint type,
                         const std::string &name)
    : id(glCreateShader(type)) {

  static const std::map<const unsigned int, std::string> shader_types{
      {GL_VERTEX_SHADER, "vertex shader"},
      {GL_FRAGMENT_SHADER, "fragment shader"},
      {GL_GEOMETRY_SHADER, "geometry shader"}};

  auto const *chars = source.c_str();
  std::cout << "Compiling: " << (!name.empty() ? name + " " : "")
            << shader_types.at(type) << std::endl;
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
    std::cerr << "Compile failure in:  " << (!name.empty() ? name + " " : "")
              << shader_types.at(type) << std::endl;
    std::cerr << std::string(buffer.begin(), buffer.end()) << std::endl;
  }
  assert(status);
}
Renderer::Shader::~Shader() { glDeleteShader(id); }

} // namespace mos::gfx
