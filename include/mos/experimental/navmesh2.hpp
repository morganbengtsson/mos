#ifndef MOS_NAVMESH2_HPP
#define MOS_NAVMESH2_HPP

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <algorithm>
#include <optional.hpp>
#include <iostream>
#include <glm/gtx/io.hpp>
#include <mos/simulation/intersection.hpp>
#include <mos/graphics/mesh.hpp>

namespace mos {

class Face2 {
public:
  Face2(const Vertex &v0, const Vertex &v1, const Vertex &v2);
  std::experimental::optional<Vertex>
  intersects(const glm::vec3 &origin, const glm::vec3 &direction);

private:
  Vertex v0_;
  Vertex v1_;
  Vertex v2_;
};

class Navmesh2 {
public:
  using OptionalIntersection = std::experimental::optional<Vertex>;
  Navmesh2();
  Navmesh2(const Mesh &mesh, const glm::mat4 &transform);

  template <class Tv, class Te>
  Navmesh2(Tv vertices_begin, Tv vertices_end, Te elements_begin,
          Te elements_end, const glm::mat4 &transform)
      : elements_(elements_begin, elements_end) {

    std::transform(vertices_begin, vertices_end, std::back_inserter(vertices_),
                   [&](const Vertex &vertex) {
                     auto v = vertex;
                     v.position = glm::vec3(transform *
                                                 glm::vec4(vertex.position, 1.0f));
                     return v;
                   });

    if (elements_.empty()) {
      for (auto it = vertices_.begin(); it != vertices_.end();) {
        auto &v0 = *it;
        it++;
        auto &v1 = *it;
        it++;
        auto &v2 = *it;
        it++;
        faces_.push_back(Face2(v0, v1, v2));
      }

    } else {
      for (auto it = elements_.begin(); it != elements_.end();) {
        auto &v0 = vertices_[*it];
        it++;
        auto &v1 = vertices_[*it];
        it++;
        auto &v2 = vertices_[*it];
        it++;
        faces_.push_back(Face2(v0, v1, v2));
      }
    }
  }
  std::experimental::optional<Vertex>
  intersects(const glm::vec3 &origin, const glm::vec3 &direction);

  std::experimental::optional<Vertex>
  closest_intersection(const glm::vec3 &origin, const glm::vec3 direction);

  ~Navmesh2();

private:
  std::vector<Face2> faces_;
  std::vector<Vertex> vertices_;
  std::vector<int> elements_;
};
}

#endif // MOS_NAVMESH2_HPP
