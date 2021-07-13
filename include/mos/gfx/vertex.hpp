#pragma once
#include <glm/glm.hpp>

namespace mos::gfx {

/** Vertex structure, supported by the renderer. */
class Vertex final {
public:
  glm::vec3 position = glm::vec3(0.0f);

  glm::vec3 normal = glm::vec3(0.0f);

  glm::vec3 tangent = glm::vec3(0.0f);

  /** Texture coordinate. */
  glm::vec2 uv = glm::vec2(0.0f);

  /** Apply a transformation matrix to the vertex */
  void apply_transform(const glm::mat4 &transform);
};
}
