#include <glm/gtx/intersect.hpp>
#include <glm/gtx/normal.hpp>
#include <mos/simulate/navmesh.hpp>

namespace mos {

Navmesh::Navmesh() {}

Navmesh::Navmesh(const gfx::Mesh &mesh, const glm::mat4 &transform)
    : Navmesh(mesh.vertices.begin(), mesh.vertices.end(), mesh.indices.begin(),
              mesh.indices.end(), transform) {}

std::experimental::optional<Intersection>
Navmesh::intersects(const glm::vec3 &origin, const glm::vec3 &direction) {
  for (auto &face : faces_) {
    auto intersection = face.intersects(origin, direction);
    if (intersection) {
      return intersection;
    }
  }
  return std::experimental::optional<Intersection>();
}

Navmesh::OptionalIntersection
Navmesh::closest_intersection(const glm::vec3 &origin,
                              const glm::vec3 direction) {
  OptionalIntersection closest;
  for (auto &face : faces_) {
    auto intersection = face.intersects(origin, direction);
    if (intersection) {
      auto distance = glm::distance(origin, intersection->position);
      if (!closest ||
          distance < glm::distance(origin, closest->position)) {
        closest = intersection;
      }
    }
  }
  return closest;
}

Navmesh::~Navmesh() {}

Face::Face(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2)
    : v0_(v0), v1_(v1), v2_(v2) {}

std::experimental::optional<Intersection>
Face::intersects(const glm::vec3 &origin, const glm::vec3 &direction) {
  glm::vec3 bary;
  auto intersects =
      glm::intersectRayTriangle(origin, direction, v0_, v1_, v2_, bary);

  if (intersects) {
    return std::experimental::optional<Intersection>(Intersection(
        origin + direction * bary.z, glm::triangleNormal(v0_, v1_, v2_)));
  } else {
    return std::experimental::optional<Intersection>();
  }
}
}
