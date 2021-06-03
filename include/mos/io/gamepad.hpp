#pragma once

#include <glm/glm.hpp>

namespace mos::io {

struct [[deprecated("Use glfw or similar directly")]] Gamepad {
  bool button_a = false;
  bool button_b = false;
  glm::vec2 left_axis = glm::vec2(0.0f);
  glm::vec2 right_axis = glm::vec2(0.0f);
};

} // namespace mos::io
