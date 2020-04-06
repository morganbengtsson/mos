#include <mos/gfx/renderer.hpp>
#include <iostream>

namespace mos::gfx {

Renderer::Program::Program() : program(glCreateProgram()) {
  assert(program);
}
Renderer::Program::~Program() {
  glDeleteProgram(program);
}
void Renderer::Program::check(const std::string &name) {
  GLint status = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    std::vector<char> buffer(length);
    glGetProgramInfoLog(program, length, nullptr, &buffer[0]);
    std::cout << "Link failure in: " << name << std::endl;
    std::cout << std::string(buffer.begin(), buffer.end()) << std::endl;
  }
  assert(status);
}
void Renderer::Program::link(const std::string &name) {
  std::cout << "Linking: " + name + " program." << std::endl;
  glLinkProgram(program);
}

}
