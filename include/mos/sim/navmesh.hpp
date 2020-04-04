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
#include <mos/gfx/assets.hpp>
#include <json.hpp>

//TODO: shared_ptr mesh
//TODO: Rename to navmodel/simmodel
namespace mos {
namespace sim {

/** Navigation mesh. */
class Navmesh {
public:
  using Optional_vertex = std::optional<gfx::Vertex>;

  Navmesh(const nlohmann::json &json, gfx::Assets &assets = *std::make_unique<gfx::Assets>(), const glm::mat4 &parent_transform = glm::mat4(1.0f));

  Optional_vertex intersects(const glm::vec3 &origin,
                             const glm::vec3 &direction);

  Optional_vertex closest_intersection(const glm::vec3 &origin,
                                       const glm::vec3 &direction);
  gfx::Shared_mesh mesh;
  glm::mat4 transform;
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
