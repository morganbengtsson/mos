#pragma once

#include <mos/gfx/mesh.hpp>
#include <mos/gpu/resource.hpp>

namespace mos::gl {
class Renderer;
} // namespace mos::gl

namespace mos::gpu {

class Mesh final : public Resource {
  friend class Model;
  friend class mos::gl::Renderer;
public:
  Mesh();
  [[nodiscard]] auto centroid() const -> glm::vec3;
  [[nodiscard]] auto radius() const -> float;
  [[nodiscard]] auto num_indices() const -> int;

private:

  Mesh(const mos::gfx::Mesh& mesh);
  Mesh(const mos::gfx::Shared_mesh &shared_mesh);
  glm::vec3 centroid_{0.0F};
  float radius_{0.0F};
  int num_indices_{0};
};
} // namespace mos::gpu
