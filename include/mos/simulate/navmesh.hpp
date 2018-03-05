#ifndef MOS_NAVMESH_HPP
#define MOS_NAVMESH_HPP

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <algorithm>
#include <experimental/optional>
#include <iostream>
#include <glm/gtx/io.hpp>
#include <mos/simulate/intersection.hpp>
#include <mos/render/mesh.hpp>

namespace mos {

class Face {
public:
  Face(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2);
  std::experimental::optional<Intersection>
  intersects(const glm::vec3 &origin, const glm::vec3 &direction);

private:
  glm::vec3 v0_;
  glm::vec3 v1_;
  glm::vec3 v2_;
};

class Navmesh {
public:
  using OptionalIntersection = std::experimental::optional<Intersection>;
  Navmesh();
  Navmesh(const gfx::Mesh &mesh, const glm::mat4 &transform);

  template <class Tv, class Te>
  Navmesh(Tv vertices_begin, Tv vertices_end, Te elements_begin,
          Te elements_end, const glm::mat4 &transform)
      : elements_(elements_begin, elements_end) {

    std::transform(vertices_begin, vertices_end, std::back_inserter(positions_),
                   [&](const gfx::Vertex &vertex) {
                     return glm::vec3(transform *
                                      glm::vec4(vertex.position, 1.0f));
                   });

    if (elements_.empty()) {
      for (auto it = positions_.begin(); it != positions_.end();) {
        auto &v0 = *it;
        it++;
        auto &v1 = *it;
        it++;
        auto &v2 = *it;
        it++;
        faces_.push_back(Face(v0, v1, v2));
      }

    } else {
      for (auto it = elements_.begin(); it != elements_.end();) {
        auto &v0 = positions_[*it];
        it++;
        auto &v1 = positions_[*it];
        it++;
        auto &v2 = positions_[*it];
        it++;
        faces_.push_back(Face(v0, v1, v2));
      }
    }
  }
  std::experimental::optional<Intersection>
  intersects(const glm::vec3 &origin, const glm::vec3 &direction);

  std::experimental::optional<Intersection>
  closest_intersection(const glm::vec3 &origin, const glm::vec3 direction);

  ~Navmesh();

private:
  std::vector<Face> faces_;
  std::vector<glm::vec3> positions_;
  std::vector<int> elements_;
};
}

#endif // MOS_NAVMESH_HPP
