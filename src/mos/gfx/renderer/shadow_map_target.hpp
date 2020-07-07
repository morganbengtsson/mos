#pragma once

#include <glad/glad.h>

struct Shadow_map_target {
  explicit Shadow_map_target(const Render_buffer &render_buffer);
  ~Shadow_map_target();
  Shadow_map_target(const Shadow_map_target &target) = delete;
  Shadow_map_target(Shadow_map_target &&target) = delete;
  Shadow_map_target &operator=(const Shadow_map_target &target) = delete;
  Shadow_map_target &operator=(Shadow_map_target &&target) = delete;
  const GLuint texture{};
  const GLuint frame_buffer{};
};
