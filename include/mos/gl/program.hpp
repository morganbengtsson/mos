#pragma once

#include <glad/glad.h>

namespace mos::gl {

class Program {
  friend class Renderer;
protected:
  Program();
  ~Program();
public:
  Program(const Program &program) = delete;
  Program(const Program &&program) = delete;
  Program &operator=(const Program &program) = delete;
  Program &operator=(const Program &&program) = delete;
  const GLuint program;
  auto check(const std::string &name) const -> void;
  auto link(const std::string &name) const -> void;
};
}
