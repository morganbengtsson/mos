#pragma once

#include <glad/glad.h>

class Shader {
public:
  Shader(const std::string &source, GLuint type, const std::string& name);
  ~Shader();
  const GLuint id;
  Shader(const Shader &shader) = delete;
  Shader(Shader &&shader) = delete;
  Shader & operator=(const Shader &shader) = delete;
  Shader & operator=(Shader &&shader) = delete;
};
