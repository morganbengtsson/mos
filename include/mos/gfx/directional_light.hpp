#pragma once

#include <glm/glm.hpp>

namespace mos::gfx {

struct Directional_light {
  glm::vec3 position{0.0f};
  glm::vec3 direction{0.0f, 0.0f, 1.0f};
  glm::vec3 color{1.0f};
  float strength{0.0f};
};
} // namespace mos::gfx
