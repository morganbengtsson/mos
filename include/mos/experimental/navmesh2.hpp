#ifndef MOS_NAVMESH2_HPP
#define MOS_NAVMESH2_HPP

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

class Face2 {
public:
  Face2(gfx::Vertex &v0, gfx::Vertex &v1, gfx::Vertex &v2);
  std::experimental::optional<gfx::Vertex>
  intersects(const glm::vec3 &origin, const glm::vec3 &direction);
public :
  gfx::Vertex v0_;
  gfx::Vertex v1_;
  gfx::Vertex v2_;
};

class Navmesh2 {
public:
  using OptionalIntersection = std::experimental::optional<gfx::Vertex>;
  Navmesh2();
  Navmesh2(const gfx::Mesh &mesh, const glm::mat4 &transform);

  template <class Tv, class Te>
  Navmesh2(Tv vertices_begin, Tv vertices_end, Te elements_begin,
          Te elements_end, const glm::mat4 &transform)
      : indices(elements_begin, elements_end), vertices(vertices_begin, vertices_end){

    for(auto & vertex : vertices) {
      vertex.position = glm::vec3(transform *
          glm::vec4(vertex.position, 1.0f));
    }
  }
  std::experimental::optional<gfx::Vertex>
  intersects(const glm::vec3 &origin, const glm::vec3 &direction);

  std::experimental::optional<gfx::Vertex>
  closest_intersection(const glm::vec3 &origin, const glm::vec3 &direction);
  void calculate_normals();

  ~Navmesh2();

  std::vector<gfx::Vertex> vertices;
  std::vector<int> indices;
};
}

#endif // MOS_NAVMESH2_HPP
