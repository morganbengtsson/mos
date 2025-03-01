#pragma once

#include <glm/glm.hpp>
#include <string>

namespace mos::gfx {

struct Directional_light {
  static auto load(const std::string &directory, const std::string &path,
                   const glm::mat4 &parent_transform = glm::mat4(1.0F))
      -> Directional_light;

  glm::vec3 position{0.0F};
  glm::vec3 direction{0.0F, 0.0F, 1.0F};
  glm::vec3 color{1.0F};
  float strength{0.0F};
};
} // namespace mos::gfx
