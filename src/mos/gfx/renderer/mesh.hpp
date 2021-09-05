#pragma once

#include <mos/gfx/mesh.hpp>

class Mesh final {
  friend class Model;
  friend class Renderer;

public:
  Mesh() = default;
  auto centroid() const -> glm::vec3;
  auto radius() const -> float;
  auto num_indices() const -> int;
  auto id() const -> int;

private:
  Mesh(const mos::gfx::Mesh& mesh);

  glm::vec3 centroid_{0.0f};
  float radius_{0.0f};
  int num_indices_{0};
  int id_{-1};
};
