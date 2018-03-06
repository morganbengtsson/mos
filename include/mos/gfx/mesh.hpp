#pragma once

#include <vector>
#include <initializer_list>
#include <string>
#include <atomic>
#include <memory>
#include <chrono>
#include <mos/gfx/vertex.hpp>
#include <mos/gfx/container.hpp>

namespace mos {
namespace gfx {
class Mesh;
using SharedMesh = std::shared_ptr<Mesh>;
/** Geometric data description. Vertices and optional indices for rendering. */
class Mesh final {
public:
  using Positions = std::vector<glm::vec3>;
  using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

  template<class Tv, class Te>
  Mesh(const Tv vertices_begin, const Tv vertices_end,
       Te elements_begin, Te elements_end)
      : vertices(vertices_begin, vertices_end),
        indices(elements_begin, elements_end), id_(current_id_++) {
  }

  Mesh(const std::initializer_list<Vertex> &vertices,
       const std::initializer_list<int> &elements);

  /** Load from *.mesh file. @param path Full path*/
  Mesh(const std::string &path);

  Mesh();

  Mesh(const Mesh &mesh);

  ~Mesh();

  static SharedMesh load(const std::string &path);

  /** @return Unique identifier. */
  unsigned int id() const;

  /** Erease all vertices and indices. */
  void clear();

  /** Get only positions from vertices */
  Positions positions() const;

  void mix(const Mesh &mesh1, const Mesh &mesh2, const float amount);

  void apply_transform(const glm::mat4 &transform);

  void calculate_normals();

  void calculate_flat_normals();

  void calculate_tangents();

  Container<Vertex> vertices;
  Container<int> indices;
private:
  void calculate_tangents(Vertex &v0, Vertex &v1, Vertex &v2);
  static std::atomic_uint current_id_;
  unsigned int id_;
};
}
}
