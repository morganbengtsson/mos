#pragma once
#include <glm/glm.hpp>

namespace mos {
namespace gfx {

/** Vertex structure, supported by the renderer. */
class Vertex final {
public:
  explicit Vertex(const glm::vec3 &position = glm::vec3(0.0f),
                  const glm::vec3 &normal = glm::vec3(0.0f),
                  const glm::vec3 &tangent = glm::vec3(0.0f),
                  const glm::vec2 &uv = glm::vec2(0.0f));

  glm::vec3 position;

  glm::vec3 normal;

  glm::vec3 tangent;

  /** Texture coordinate. */
  glm::vec2 uv;

  /** Apply a transformation matrix to the vertex */
  void apply_transform(const glm::mat4 &transform);
};
}
}

