#ifndef MOS_BOX_HPP
#define MOS_BOX_HPP

#include <array>
#include <utility>
#include <algorithm>
#include <atomic>
#include <tuple>
#include <iostream>
#include <optional.hpp>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <mos/render/vertex.hpp>
#include <mos/simulate/ray.hpp>
#include <mos/render/model.hpp>

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
 * @brief The Box class
 *
 * Axis aligned bounding box class. Used for colission detection and sound
 *obstruction.
 */
class Box {
public:
  using OptionalIntersection = std::experimental::optional<glm::vec3>;
  template <class T> Box(const T &positions, const glm::mat4 &transform) {
    std::vector<glm::vec3> transformed;

    position = glm::vec3(transform[3][0], transform[3][1], transform[3][2]);

    std::transform(positions.begin(), positions.end(),
                   std::back_inserter(transformed),
                   [&](const glm::vec3 &position) {
                     return glm::vec3(transform * glm::vec4(position, 1.0f));
                   });

    auto min_max = min_max_positions(transformed.begin(), transformed.end());

    extent = (min_max.second - min_max.first) / 2.0f;
    position = min_max.first + extent;
  }

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
  Box(VertexIt begin, VertexIt end, const glm::mat4 &transform) {
    glm::vec3 min, max;

    position = glm::vec3(transform[3][0], transform[3][1], transform[3][2]);

    std::vector<glm::vec3> positions;
    std::transform(
        begin, end, std::back_inserter(positions), [&](const Vertex &vertex) {
          return glm::vec3(transform * glm::vec4(vertex.position, 1.0f));
        });

    auto min_max = min_max_positions(positions.begin(), positions.end());
    extent = (min_max.second - min_max.first) / 2.0f;
    position = min_max.first + extent;
  }

  /**
   * @brief Box
   * @param extent
   * @param position
   */
  Box(const glm::vec3 &extent, const glm::vec3 &position);

  /**
   * @brief Box default constructor.
   */
  Box();

  static Box create_from_model(const Model &model, const glm::mat4 &transform = glm::mat4(1.0f));
  static Box create_from_min_max(const glm::vec3& min, const glm::vec3 &max);


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

  /**
   * @brief Intersection with ray
   * @param origin
   * @param direction

   * @return
   */
  bool intersects(const glm::vec3 &origin, const glm::vec3 &direction) const;

  /**
   * @brief intersect
   * @param ray
   * @return
   */
  bool intersects(const Ray &ray) const;

  /**
   * @brief intersects
   * @param other
   * @return
   */
  [[deprecated]]
  std::experimental::optional<BoxIntersection>
  intersects(const Box &other) const;

  /**
   * @brief Simple intersect method.
   * @param other
   * @return
   */
  [[deprecated]]
  glm::vec3 intersects_simple(const Box &other);

  /**
   * @brief intersection
   * @param other
   * @return
   */
  OptionalIntersection intersection(const Box &other) const;

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
   * @brief id
   * @return
   */
  unsigned int id() const;

  /**
   * @brief Size of box.
   * @return size in x, y and z directions.
   */
  glm::vec3 size() const;

  /**
   *
   * @param os
   * @param box
   * @return
   */
  friend std::ostream &operator<<(std::ostream &os, const Box &box);

  /**
   * @brief extent
   */
  glm::vec3 extent;

  /**
   * @brief position
   */
  glm::vec3 position;

private:
  static std::atomic_uint current_id_;
  unsigned int id_;

  template <class T>
  std::pair<glm::vec3, glm::vec3> min_max_positions(T begin, T end) const {
    std::pair<glm::vec3, glm::vec3> m;
    auto x_extremes = std::minmax_element(
        begin, end, [](const glm::vec3 &left, const glm::vec3 &right) {
          return left.x < right.x;
        });

    auto y_extremes = std::minmax_element(
        begin, end, [](const glm::vec3 &left, const glm::vec3 &right) {
          return left.y < right.y;
        });

    auto z_extremes = std::minmax_element(
        begin, end, [](const glm::vec3 &left, const glm::vec3 &right) {
          return left.z < right.z;
        });

    m.first = glm::vec3(x_extremes.first->x, y_extremes.first->y,
                        z_extremes.first->z);
    m.second = glm::vec3(x_extremes.second->x, y_extremes.second->y,
                         z_extremes.second->z);
    return m;
  }
};
}
#endif // MOS_BOX_HPP
