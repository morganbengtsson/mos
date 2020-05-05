#pragma once

#include <vector>
#include <initializer_list>
#include <string>
#include <atomic>
#include <memory>
#include <chrono>
#include <mos/gfx/vertex.hpp>
#include <mos/gfx/shape.hpp>
#include <mos/core/tracked_container.hpp>

namespace mos::gfx {

class Mesh;
using Shared_mesh = std::shared_ptr<Mesh>;
using Triangle = std::array<int, 3>;

/** Geometric data description, vertices and indices. */
class Mesh final : public Shape {
public:
  using Positions = std::vector<glm::vec3>;
  using Time_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

  template<class Tv, class Te>
  Mesh(const Tv vertices_begin, const Tv vertices_end,
       Te triangles_begin, Te triangles_end)
      : vertices(vertices_begin, vertices_end),
        triangles(triangles_begin, triangles_end) {
    calculate_sphere();
  }

  Mesh(const std::initializer_list<Vertex> &vertices,
       const std::initializer_list<Triangle> &triangles);

  /** Load from *.mesh file. @param path Full path*/
  explicit Mesh(const std::string &path);

  Mesh();

  static auto load(const std::string &path) -> Shared_mesh;

  /** Erease all vertices and indices. */
  auto clear() -> void;

  /** Get only positions from vertices */
  auto positions() const -> Positions;

  auto mix(const Mesh &mesh1, const Mesh &mesh2, float amount) -> void;

  auto apply_transform(const glm::mat4 &transform) -> void;

  auto calculate_normals() -> void;

  auto calculate_flat_normals() -> void;

  auto calculate_tangents() -> void;

  auto calculate_sphere() -> void;

  glm::vec3 centroid{0.0f};
  float radius{0.0f};

  Tracked_container<Vertex> vertices;
  Tracked_container<Triangle> triangles;
private:
  static auto calculate_tangents(Vertex &v0, Vertex &v1, Vertex &v2) -> void;

  struct Face {
    Vertex &v0;
    Vertex &v1;
    Vertex &v2;
    auto normal() const -> glm::vec3;
  };
};
}

