#pragma once

#include <glm/glm.hpp>
#include <string>

namespace mos::gfx {

struct Directional_light {
  static auto load(const std::string &directory, const std::string &path,
                   const glm::mat4 &parent_transform = glm::mat4(1.0f))
      -> Directional_light;

  glm::vec3 position{0.0f};
  glm::vec3 direction{0.0f, 0.0f, 1.0f};
  glm::vec3 color{1.0f};
  float strength{0.0f};
};
} // namespace mos::gfx
