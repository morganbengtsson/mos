#pragma once

#include <mos/gfx/mesh.hpp>
#include <mos/gfx/renderer.hpp>
#include <mos/gfx/gpu/resource.hpp>

namespace mos::gfx::gpu {

class Mesh final : public Resource {
  friend class Model;
  friend class mos::gfx::Renderer;

public:
  Mesh();
  auto centroid() const -> glm::vec3;
  auto radius() const -> float;
  auto num_indices() const -> int;

private:
  Mesh(const mos::gfx::Mesh& mesh);
  Mesh(const mos::gfx::Shared_mesh &shared_mesh);

  glm::vec3 centroid_{0.0f};
  float radius_{0.0f};
  int num_indices_{0};
};
}
