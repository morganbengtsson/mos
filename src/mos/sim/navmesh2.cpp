#include <glm/gtx/intersect.hpp>
#include <glm/gtx/normal.hpp>
#include <mos/sim/navmesh2.hpp>

namespace mos {
namespace sim {

Navmesh2::Navmesh2() {}

Navmesh2::Navmesh2(const gfx::Mesh &mesh, const glm::mat4 &transform)
    : Navmesh2(mesh.vertices.begin(), mesh.vertices.end(), mesh.indices.begin(),
               mesh.indices.end(), transform) {}

std::experimental::optional<gfx::Vertex>
Navmesh2::intersects(const glm::vec3 &origin, const glm::vec3 &direction) {
  //for (auto &face : faces_) {
  for (int i = 0; i < indices.size(); i += 3) {
    Face2 face(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
    auto intersection = face.intersects(origin, direction);
    if (intersection) {
      return intersection;
    }
  }
  return std::experimental::optional<gfx::Vertex>();
}

Navmesh2::OptionalIntersection
Navmesh2::closest_intersection(const glm::vec3 &origin,
                               const glm::vec3 &direction) {
  OptionalIntersection closest;
  //for (auto &face : faces_) {
  for (int i = 0; i < indices.size(); i += 3) {
    Face2 face(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
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
void Navmesh2::calculate_normals() {
  for (int i = 0; i < indices.size(); i += 3) {
    //TODO: Generalize
    auto &v0 = vertices[indices[i]];
    auto &v1 = vertices[indices[i + 1]];
    auto &v2 = vertices[indices[i + 2]];

    auto normal = glm::triangleNormal(v0.position, v1.position, v2.position);
    v0.normal = normal;
    v1.normal = normal;
    v2.normal = normal;
  }
}

Face2::Face2(gfx::Vertex &v0, gfx::Vertex &v1, gfx::Vertex &v2)
    : v0_(v0), v1_(v1), v2_(v2) {}

std::experimental::optional<gfx::Vertex>
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

    //n = a * v0_.normal + b * v1_.normal + c * v2_.normal;

    n = glm::normalize(n);



    //auto uv = (v0_.uv + v1_.uv + v2_.uv) / 3.0f;
    auto uv = a * v0_.uv + b * v1_.uv + c * v2_.uv;

    //auto uv_l = bary.x * v0_.uv_lightmap + bary.y * v1_.uv_lightmap + bary.z * v2_.uv_lightmap;
    auto v = gfx::Vertex(p, n, t, uv);
    return std::experimental::optional<gfx::Vertex>(v);
  } else {
    return std::experimental::optional<gfx::Vertex>();
  }
}
}
}
