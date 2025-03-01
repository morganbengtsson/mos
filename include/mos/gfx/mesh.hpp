#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

#include <mos/core/tracked_container.hpp>
#include <mos/gfx/shape.hpp>
#include <mos/gfx/vertex.hpp>

namespace mos::gfx {

class Mesh;
using Shared_mesh = std::shared_ptr<Mesh>;
using Triangle_indices = std::array<int, 3>;

/** Geometric data description, vertices and indices. */
class Mesh final : public Shape {
public:
  using Positions = std::vector<glm::vec3>;
  using Time_point = std::chrono::time_point<std::chrono::system_clock,
                                             std::chrono::nanoseconds>;

  template <class Tv, class Te>
  explicit Mesh(const Tv vertices_begin, const Tv vertices_end,
                Te indices_begin, Te indices_end)
      : vertices(vertices_begin, vertices_end),
        indices(indices_begin, indices_end) {
    calculate_sphere();
  }

  explicit Mesh(const std::initializer_list<Vertex> &vertices,
                const std::initializer_list<Triangle_indices> &triangles);

  explicit Mesh();

  /** Load from *.mesh file. @param path Full path*/
  static auto load(const std::string &path) -> Mesh;

  /** Erease all vertices and indices. */
  auto clear() -> void;

  /** Get only positions from vertices */
  [[nodiscard]] auto positions() const -> Positions;

  auto mix(const Mesh &mesh1, const Mesh &mesh2, float amount) -> void;

  auto apply_transform(const glm::mat4 &transform) -> void;

  auto calculate_normals() -> void;

  auto calculate_flat_normals() -> void;

  auto calculate_tangents() -> void;

  auto calculate_sphere() -> void;

  [[nodiscard]] auto centroid() const -> glm::vec3;

  [[nodiscard]] auto radius() const -> float;

  Tracked_container<Vertex> vertices;
  Tracked_container<Triangle_indices> indices;

private:
  struct Triangle {
    Vertex &v0;
    Vertex &v1;
    Vertex &v2;
    [[nodiscard]] auto normal() const -> glm::vec3;
  };

  static auto calculate_tangents(Vertex &v0, Vertex &v1, Vertex &v2) -> void;
  void for_each_triangle(
      const std::function<void(const Triangle &triangle)> &callback);

  glm::vec3 centroid_{0.0F};
  float radius_{0.0F};
};
} // namespace mos::gfx
