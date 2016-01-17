#ifndef MOS_BOX_HPP
#define MOS_BOX_HPP

#include <array>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include <algorithm>
#include <tuple>
#include <iostream>
#include <optional.hpp>
#include <mos/graphics/vertex.hpp>

namespace mos {

/**
 * @brief The BoxIntersection struct
 * Data from a box box intersection.
 *
 */
struct BoxIntersection {
  BoxIntersection(const glm::vec3 &normal, const float distance)
      : normal(normal), distance(distance) {}

  /**
   * @brief Normal of the intersection.
   */
  glm::vec3 normal;
  /**
   * @brief Distance of the intersection.
   */
  float distance;
};

/**
 * @brief The RayIntersection struct
 * Intersection data from a ray and box intersection.
 *
 */
struct RayIntersection {
  /**
   * @brief RayIntersection constructor.
   * @param hit
   * @param distance
   */
  RayIntersection(float distance = 0.0f) : distance(distance) {}

  /**
   * @brief distance
   *
   * Distance from
   */
  float distance;
};

/**
 * @brief The Box class
 *
 * Axis aligned bounding box class. Used for colission detection and sound
 *obstruction.
 */
class Box {
public:
  template <class VertexIt>
  /**
   * @brief Box constructor.
   * @param begin Vertex iterator.
   * @param end Vertex iterator.
   * @param transform Location of the box.
   * @param obstruction Obstruction factor for sound.
   *
   * Constructor for an AABB box, created from vertices.
   *
   */
  Box(VertexIt begin, VertexIt end, const glm::mat4 &transform,
      const float obstruction = 0.0f, const bool step = false)
      : obstruction_(obstruction), step_(step) {
    glm::vec3 min, max;

    glm::vec3 position(transform[3][0], transform[3][1], transform[3][2]);

    std::vector<glm::vec3> positions;
    std::transform(
        begin, end, std::back_inserter(positions), [&](const Vertex &vertex) {
          return glm::vec3(transform * glm::vec4(vertex.position, 1.0f));
        });

    if (positions.begin() != positions.end()) {
      auto x_extremes = std::minmax_element(
          positions.begin(), positions.end(),
          [](const glm::vec3 &left, const glm::vec3 &right) {
            return left.x < right.x;
          });

      auto y_extremes = std::minmax_element(
          positions.begin(), positions.end(),
          [](const glm::vec3 &left, const glm::vec3 &right) {
            return left.y < right.y;
          });

      auto z_extremes = std::minmax_element(
          positions.begin(), positions.end(),
          [](const glm::vec3 &left, const glm::vec3 &right) {
            return left.z < right.z;
          });

      min = glm::vec3(x_extremes.first->x, y_extremes.first->y,
                      z_extremes.first->z);
      max = glm::vec3(x_extremes.second->x, y_extremes.second->y,
                      z_extremes.second->z);
    }
    extent_ = (max - min) / 2.0f;
    // position_ = position;
    position_ = min + extent_;
  }

  /**
   * @brief Box default constructor.
   */
  Box();

  /**
   * @brief min
   * @return Min corner.
   */
  glm::vec3 min() const;

  /**
   * @brief max
   * @return Max corner.
   */
  glm::vec3 max() const;

#if 0
    /**
     * @brief Intersection with ray between points.
     * @param point1
     * @param point2
     * @return
     */
    RayIntersection intersect(glm::vec3 point1, glm::vec3 point2);

    /**
     * @brief Intersection with ray
     * @param origin
     * @param direction
     * @param t1
     * @param t2
     * @return
     */
    RayIntersection intersect(const glm::vec3 & origin, const glm::vec3 direction, float t1, float t2);
#endif
  /**
   * @brief intersects
   * @param other
   * @return
   */
  std::experimental::optional<BoxIntersection>
  intersects(const Box &other) const;

  /**
   * @brief Simple intersect method.
   * @param other
   * @return
   */
  glm::vec3 intersects_simple(const Box &other);

  /**
   * @brief position
   * @return
   */
  glm::vec3 position() const;

  /**
   * @brief position
   * @param position
   */
  void position(const glm::vec3 &position);

  /**
   * @brief Set the box transform, only uses position elements.
   * @param transform
   */
  void transform(const glm::mat4 &transform);

  /**
   * @brief Get box volume.
   */
  float volume() const;

  /**
   * @brief Obstruction factor for sound.
   *
   * Between 0.0 - 1.0
   */
  float obstruction() const;

  /**
   * @brief Size of box.
   * @return size in x, y and z directions.
   */
  glm::vec3 size() const;

  /**
   * @brief step
   * @param step
   */
  void step(const bool step);
  bool step() const;

private:
  glm::vec3 extent_;
  glm::vec3 position_;
  float obstruction_;
  bool step_;
};
}
#endif // MOS_BOX_HPP
