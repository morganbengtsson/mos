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
                  const glm::vec2 &uv = glm::vec2(0.0f),
                  float weight = 1.0f);

  ~Vertex() = default;

  glm::vec3 position;

  glm::vec3 normal;

  glm::vec3 tangent;

  glm::vec2 uv;

  float weight;
};
}
}

