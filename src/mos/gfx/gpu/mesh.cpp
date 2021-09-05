#include <mos/gfx/renderer.hpp>
#include <mos/gfx/gpu/mesh.hpp>

mos::gfx::gpu::Mesh::Mesh(const mos::gfx::Mesh& mesh)
    : centroid_(mesh.centroid()), radius_(mesh.radius()),
      num_indices_(mesh.indices.size()), id_(mesh.id()) {}

auto mos::gfx::gpu::Mesh::radius() const -> float {
  return radius_;
}

auto mos::gfx::gpu::Mesh::num_indices() const -> int {
  return num_indices_;
}

auto mos::gfx::gpu::Mesh::id() const -> int {
  return id_;
}

auto mos::gfx::gpu::Mesh::centroid() const -> glm::vec3 {
  return centroid_;
}
