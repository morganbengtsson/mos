#pragma once
#include <glm/glm.hpp>

namespace mos {
namespace gfx {

/** The vertex structure, supported by the renderer. */
class Vertex final {
public:
  explicit Vertex(const glm::vec3 &position = glm::vec3(0.0f),
                  const glm::vec3 &normal = glm::vec3(0.0f),
                  const glm::vec3 &tangent = glm::vec3(0.0f),
                  const glm::vec2 &uv = glm::vec2(0.0f),
                  const float ao = 1.0f);

  ~Vertex();

  /**
   * @brief operator +
   * @param vertex
   * @return
   */
  Vertex operator+(const Vertex &vertex) const;

  /**
   * @brief operator -
   * @param vertex
   * @return
   */
  Vertex operator-(const Vertex &vertex) const;

  /**
   * @brief operator *
   * @param vertex
   * @return
   */
  Vertex operator*(const Vertex &vertex) const;

  /**
   * @brief operator *
   * @param number
   * @return
   */
  Vertex operator*(const float number) const;

  /**
   * @brief operator /
   * @param number
   * @return
   */
  Vertex operator/(const float number) const;

  /**
   * @brief operator /
   * @param vertex
   * @return
   */
  Vertex operator/(const Vertex &vertex) const;

  glm::vec3 position;

  glm::vec3 normal;

  glm::vec3 tangent;

  glm::vec2 uv;

  float ao;
};
}
}

