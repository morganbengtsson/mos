#pragma once

#include <glm/glm.hpp>

namespace mos {
namespace gfx {

/** The vertex structure, supported by the renderer. */
class Vertex {
public:
  /*!
   * @brief Vertex constructor
   * @param position
   * @param normal
   * @param uv
   *
   * Constructs a vertex. Probably the smallest primitive in this context.
   *
   */
  explicit Vertex(const glm::vec3 &position = glm::vec3(0.0f),
                  const glm::vec3 &normal = glm::vec3(0.0f),
                  const glm::vec3 &tangent = glm::vec3(0.0f),
                  const glm::vec2 &uv = glm::vec2(0.0f),
                  const float ao = 1.0f);

  /**
   * @brief Destructor.
  */
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

  /**
   * @brief position
   */
  glm::vec3 position;

  /**
   * @brief normal
   */
  glm::vec3 normal;

  /**
   * @brief tangent
   */
  glm::vec3 tangent;

  /**
   * @brief Texture cordinate for color.
   */
  glm::vec2 uv;

  /**
  * @brief Ambient occlusion.
  */
  float ao;
};
}
}

