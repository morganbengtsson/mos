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

namespace mos {
namespace gfx {

class Mesh;
using SharedMesh = std::shared_ptr<Mesh>;
using Triangle = std::array<int, 3>;
/** Geometric data description. Vertices and optional indices for rendering. */
class Mesh final : public Shape {
public:
  using Positions = std::vector<glm::vec3>;
  using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

  template<class Tv, class Te>
  Mesh(const Tv vertices_begin, const Tv vertices_end,
       Te triangles_begin, Te triangles_end)
      : vertices(vertices_begin, vertices_end),
        triangles(triangles_begin, triangles_end) {
  }

  Mesh(const std::initializer_list<Vertex> &vertices,
       const std::initializer_list<Triangle> &triangles);

  /** Load from *.mesh file. @param path Full path*/
  Mesh(const std::string &path);

  Mesh();

  Mesh(const Mesh &mesh);

  ~Mesh();

  static SharedMesh load(const std::string &path);

  /** Erease all vertices and indices. */
  void clear();

  /** Get only positions from vertices */
  Positions positions() const;

  void mix(const Mesh &mesh1, const Mesh &mesh2, const float amount);

  void apply_transform(const glm::mat4 &transform);

  void calculate_normals();

  void calculate_flat_normals();

  void calculate_tangents();

  TrackedContainer<Vertex> vertices;
  TrackedContainer<Triangle> triangles;
private:
  void calculate_tangents(Vertex &v0, Vertex &v1, Vertex &v2);
};
}
}
