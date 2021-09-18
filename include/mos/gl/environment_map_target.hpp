#pragma once

#include <glad/glad.h>

namespace mos::gl {

class Environment_map_target {
  friend class Renderer;
private:
  explicit Environment_map_target(const Render_buffer &render_buffer);
public:
  ~Environment_map_target();
  Environment_map_target(const Environment_map_target &target) = delete;
  Environment_map_target(Environment_map_target &&target) = delete;
  Environment_map_target &
  operator=(const Environment_map_target &target) = delete;
  Environment_map_target &operator=(Environment_map_target &&target) = delete;
  const GLuint texture{};
  const GLuint albedo{};
  const GLuint frame_buffer{};
};
}
