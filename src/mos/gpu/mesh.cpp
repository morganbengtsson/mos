#include <mos/gpu/mesh.hpp>

mos::gpu::Mesh::Mesh(const mos::gfx::Mesh& mesh)
    : Resource(mesh.id()), centroid_(mesh.centroid()), radius_(mesh.radius()),
      num_indices_(mesh.indices.size()) {}

mos::gpu::Mesh::Mesh(const gfx::Shared_mesh &shared_mesh) : Mesh(shared_mesh ? *shared_mesh : Mesh()){

}

auto mos::gpu::Mesh::radius() const -> float {
  return radius_;
}

auto mos::gpu::Mesh::num_indices() const -> int {
  return num_indices_;
}

mos::gpu::Mesh::Mesh() : Resource(invalid_id)
{
}

auto mos::gpu::Mesh::centroid() const -> glm::vec3 {
  return centroid_;
}
