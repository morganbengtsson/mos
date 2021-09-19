#pragma once

#include <glad/glad.h>
#include <string>

namespace mos::gl {

class Shader {
  friend class Renderer;
  friend class Bloom_program;
  friend class Cloud_program;
  friend class Box_program;
  friend class Compositing_program;
  friend class Depth_program;
  friend class Blur_program;
  friend class Standard_program;
private:
  Shader(const std::string &source, GLuint type, const std::string &name);
  Shader(const std::string &name, GLuint type);
public:
  ~Shader();
  const GLuint id;
  Shader(const Shader &shader) = delete;
  Shader(Shader &&shader) = delete;
  Shader &operator=(const Shader &shader) = delete;
  Shader &operator=(Shader &&shader) = delete;
};
}
