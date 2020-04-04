#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <algorithm>
#include <optional>
#include <iostream>
#include <glm/gtx/io.hpp>
#include <mos/sim/intersection.hpp>
#include <mos/gfx/mesh.hpp>

//TODO: shared_ptr mesh
//TODO: save transform
//TODO: Rename to navmodel/simmodel
namespace mos {
namespace sim {

/** Navigation mesh. */
class Navmesh {
public:
  using OptionalIntersection = std::optional<gfx::Vertex>;
  Navmesh();
  Navmesh(const gfx::Mesh &mesh, const glm::mat4 &transform);

  template<class Tv, class Te>
  Navmesh(Tv vertices_begin, Tv vertices_end, Te elements_begin,
           Te elements_end, const glm::mat4 &transform)
      : mesh(std::make_shared<gfx::Mesh>(vertices_begin, vertices_end, elements_begin, elements_end)) {

    for (auto &vertex : mesh->vertices) {
      vertex.position = glm::vec3(transform *
          glm::vec4(vertex.position, 1.0f));
    }
  }
  std::optional<gfx::Vertex>
  intersects(const glm::vec3 &origin, const glm::vec3 &direction);

  std::optional<gfx::Vertex>
  closest_intersection(const glm::vec3 &origin, const glm::vec3 &direction);
  void calculate_normals();

  gfx::Shared_mesh mesh;

private:
  class Face {
  public:
    Face(gfx::Vertex &v0, gfx::Vertex &v1, gfx::Vertex &v2);
    std::optional<gfx::Vertex>
    intersects(const glm::vec3 &origin, const glm::vec3 &direction);
  public :
    gfx::Vertex v0_;
    gfx::Vertex v1_;
    gfx::Vertex v2_;
  };
};
}
}
