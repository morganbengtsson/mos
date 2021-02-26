#include <iostream>
#include <mos/gfx/gl/renderer.hpp>

namespace mos::gfx {

Renderer::Program::Program() : program(glCreateProgram()) { assert(program); }
Renderer::Program::~Program() { glDeleteProgram(program); }

auto Renderer::Program::check(const std::string &name) const -> void {
  GLint status = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint length{1};
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    std::vector<char> buffer(length);
    glGetProgramInfoLog(program, length, nullptr, &buffer[0]);
    std::cout << "Link failure in: " << name << std::endl;
    std::cout << std::string(buffer.begin(), buffer.end()) << std::endl;
  }
  assert(status);
}

auto Renderer::Program::link(const std::string &name) const -> void {
  std::cout << "Linking: " + name + " program." << std::endl;
  glLinkProgram(program);
}

} // namespace mos::gfx
