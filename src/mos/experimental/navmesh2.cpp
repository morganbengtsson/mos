#include <glm/gtx/intersect.hpp>
#include <glm/gtx/normal.hpp>
#include <mos/experimental/navmesh2.hpp>

namespace mos {

Navmesh2::Navmesh2() {}

Navmesh2::Navmesh2(const Mesh &mesh, const glm::mat4 &transform)
    : Navmesh2(mesh.vertices_begin(), mesh.vertices_end(), mesh.elements_begin(),
              mesh.elements_end(), transform) {}

std::experimental::optional<Vertex>
Navmesh2::intersects(const glm::vec3 &origin, const glm::vec3 &direction) {
  for (auto &face : faces_) {
    auto intersection = face.intersects(origin, direction);
    if (intersection) {
      return intersection;
    }
  }
  return std::experimental::optional<Vertex>();
}

Navmesh2::OptionalIntersection
Navmesh2::closest_intersection(const glm::vec3 &origin,
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

Navmesh2::~Navmesh2() {}

Face2::Face2(const Vertex &v0, const Vertex &v1, const Vertex &v2)
    : v0_(v0), v1_(v1), v2_(v2) {}

std::experimental::optional<Vertex>
Face2::intersects(const glm::vec3 &origin, const glm::vec3 &direction) {
  glm::vec3 bary;
  auto intersects =
      glm::intersectRayTriangle(origin, direction, v0_.position, v1_.position, v2_.position, bary);

  if (intersects) {
    auto p = origin + direction * bary.z;
    auto n = glm::triangleNormal(v0_.position, v1_.position, v2_.position);
    auto t = glm::normalize(v0_.position - v1_.position); // Is this correct?

    //Document this?
    auto a = bary.x / (bary.x + bary.y + bary.z);
    auto b = bary.y / (bary.x + bary.y + bary.z);
    auto c = bary.z / (bary.x + bary.y + bary.z);

    //auto uv = (v0_.uv + v1_.uv + v2_.uv) / 3.0f;
    auto uv = a * v0_.uv + b * v1_.uv + c * v2_.uv;

    //auto uv_l = (v0_.uv_lightmap + v1_.uv_lightmap + v2_.uv_lightmap) / 3.0f;
    auto uv_l = a * v0_.uv_lightmap + b * v1_.uv_lightmap + c * v2_.uv_lightmap;

    //auto uv_l = bary.x * v0_.uv_lightmap + bary.y * v1_.uv_lightmap + bary.z * v2_.uv_lightmap;
    auto v = Vertex(p, n, t, uv, uv_l);
    return std::experimental::optional<Vertex>(v);
  } else {
    return std::experimental::optional<Vertex>();
  }
}
}
