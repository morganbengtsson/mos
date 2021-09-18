#include <iostream>
#include <mos/gl/renderer.hpp>
#include <spdlog/spdlog.h>

namespace mos::gl {

Program::Program() : program(glCreateProgram()) { assert(program); }
Program::~Program() { glDeleteProgram(program); }

auto Program::check(const std::string &name) const -> void {
  GLint status = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint length{1};
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    std::vector<char> buffer(length);
    glGetProgramInfoLog(program, length, nullptr, &buffer[0]);
    spdlog::error("Link failure in: {} \n {}", name, std::string(buffer.begin(), buffer.end()));
  }
  assert(status);
}

auto Program::link(const std::string &name) const -> void {
  spdlog::info("Linking: {} program", name);
  glLinkProgram(program);
}

} // namespace mos::gfx
