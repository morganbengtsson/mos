#include <mos/gfx/gpu/mesh.hpp>

mos::gfx::gpu::Mesh::Mesh(const mos::gfx::Mesh& mesh)
    : Resource(mesh.id()), centroid_(mesh.centroid()), radius_(mesh.radius()),
      num_indices_(mesh.indices.size()) {}

mos::gfx::gpu::Mesh::Mesh(const Shared_mesh &shared_mesh) : Mesh(shared_mesh ? *shared_mesh : Mesh()){

}

auto mos::gfx::gpu::Mesh::radius() const -> float {
  return radius_;
}

auto mos::gfx::gpu::Mesh::num_indices() const -> int {
  return num_indices_;
}

mos::gfx::gpu::Mesh::Mesh() : Resource(invalid_id)
{
}

auto mos::gfx::gpu::Mesh::centroid() const -> glm::vec3 {
  return centroid_;
}
