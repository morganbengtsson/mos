#pragma once

#include <glad/glad.h>

namespace mos::gl {

class Shadow_map_target {
  friend class Renderer;
private:
  explicit Shadow_map_target(const Render_buffer &render_buffer);
public:
  ~Shadow_map_target();
  Shadow_map_target(const Shadow_map_target &target) = delete;
  Shadow_map_target(Shadow_map_target &&target) = delete;
  Shadow_map_target &operator=(const Shadow_map_target &target) = delete;
  Shadow_map_target &operator=(Shadow_map_target &&target) = delete;
  const GLuint texture{};
  const GLuint frame_buffer{};
};
}
