#pragma once

#include <glad/glad.h>

namespace mos::gl {

class Standard_target {
  friend class Renderer;
private:
  Standard_target(const glm::ivec2 &resolution, const int samples);
public:
  ~Standard_target();
  Standard_target(const Standard_target &target) = delete;
  Standard_target(Standard_target &&target) = delete;
  Standard_target &operator=(const Standard_target &target) = delete;
  Standard_target &operator=(Standard_target &&target) = delete;
  const GLuint frame_buffer{};
  const GLuint color_texture{};
  const GLuint depth_texture{};
};
}
