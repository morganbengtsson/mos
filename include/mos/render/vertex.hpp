#ifndef MOS_VERTEX_H
#define MOS_VERTEX_H

#include <glm/glm.hpp>

namespace mos {

/*!
 * The vertex structure, supported by the renderer.
 */
class Vertex {
public:
  /*!
   * @brief Vertex constructor
   * @param position
   * @param normal
   * @param uv
   * @param uv_lightmap
   *
   * Constructs a vertex. Probably the smallest primitive in this context.
   *
   */
  explicit Vertex(const glm::vec3 position = glm::vec3(0.0f),
                  const glm::vec3 normal = glm::vec3(0.0f),
                  const glm::vec2 uv = glm::vec2(0.0f),
                  const glm::vec2 uv_lightmap = glm::vec2(0.0f));


  /**
   * @brief Vertex copy constructor.
   * @param vertex to copy.
   */
  /*
  Vertex(const Vertex& vertex);*/

  /**
   * @brief Vertex constructor.
   * @param position_x
   * @param position_y
   * @param position_z
   * @param normal_x
   * @param normal_y
   * @param normal_z
   * @param uv_x
   * @param uv_y
   * @param uv_lightmap_x
   * @param uv_lightmap_y
   */
  explicit Vertex(const float position_x, const float position_y,
                  const float position_z, const float normal_x = 0.0f,
                  const float normal_y = 0.0f, const float normal_z = 0.0f,
                  const float uv_x = 0.0f, const float uv_y = 0.0f,
                  const float uv_lightmap_x = 0.0f,
                  const float uv_lightmap_y = 0.0f);
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
   * @brief Texture cordinate for color.
   */
  glm::vec2 uv;

  /**
   * @brief Texture coordinate for the lightmap.
   */
  glm::vec2 uv_lightmap;
};
}

#endif /* MOS_VERTEX_H */
