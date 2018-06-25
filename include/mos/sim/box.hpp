#pragma once
#include <array>
#include <utility>
#include <algorithm>
#include <atomic>
#include <tuple>
#include <iostream>
#include <optional>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <mos/gfx/vertex.hpp>
#include <mos/sim/ray.hpp>
#include <mos/gfx/model.hpp>

namespace mos {
namespace sim {

/** Axis aligned bounding box. Used for colission detection and sound obstruction. */
class Box {
public:
  template<class T> Box(const T &positions, const glm::mat4 &transform) {
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

  template<class VertexIt>
  /** Create a box from vertices and a transform. */
  Box(VertexIt begin, VertexIt end, const glm::mat4 &transform) {
    glm::vec3 min, max;

    position = glm::vec3(transform[3][0], transform[3][1], transform[3][2]);

    std::vector<glm::vec3> positions;
    std::transform(
        begin, end, std::back_inserter(positions), [&](const gfx::Vertex &vertex) {
          return glm::vec3(transform * glm::vec4(vertex.position, 1.0f));
        });

    auto min_max = min_max_positions(positions.begin(), positions.end());
    extent = (min_max.second - min_max.first) / 2.0f;
    position = min_max.first + extent;
  }

  Box(const glm::vec3 &extent, const glm::vec3 &position);

  Box();

  static Box create_from_model(const gfx::Model &model, const glm::mat4 &transform = glm::mat4(1.0f));
  static Box create_from_min_max(const glm::vec3 &min, const glm::vec3 &max);

  glm::vec3 min() const;

  glm::vec3 max() const;

  bool intersects(const glm::vec3 &origin, const glm::vec3 &direction) const;

  bool intersects(const Ray &ray) const;

  bool intersect2(const Box &other) const {
    auto mmax = max();
    auto mmin = min();
    auto other_min = other.min();
    auto other_max = other.max();
    if (mmax.x < other_min.x)
      return false;
    if (mmin.x > other_max.x)
      return false;
    if (mmax.y < other_min.y)
      return false;
    if (mmin.y > other_max.y)
      return false;
    if (mmax.z < other_min.z)
      return false;
    if (mmin.z > other_max.z)
      return false;
    return true; // boxes overlap
  }

  void transform(const glm::mat4 &transform);

  /** Get box volume. */
  float volume() const;

  glm::vec3 size() const;

  friend std::ostream &operator<<(std::ostream &os, const Box &box);

  bool operator==(const Box &other) const {
    return position == other.position && extent == other.extent;
  }

  bool operator!=(const Box &other) const {
    return position != other.position || extent != other.extent;
  }

  glm::vec3 extent;
  glm::vec3 position;

private:
  template<class T>
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
}