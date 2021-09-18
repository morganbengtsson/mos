#pragma once

#include <mos/gfx/mesh.hpp>
#include <mos/gpu/resource.hpp>

namespace mos::gl {
class Renderer;
}

namespace mos::gpu {

class Mesh final : public Resource {
  friend class Model;
  friend class mos::gl::Renderer;
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
