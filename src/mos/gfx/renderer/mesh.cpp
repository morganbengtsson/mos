#include <mos/gfx/renderer.hpp>
#include "mesh.hpp"

mos::gfx::Renderer::Mesh::Mesh(const mos::gfx::Mesh& mesh)
    : centroid(mesh.centroid), radius(mesh.radius),
      indices_size(mesh.indices.size()), id(mesh.id()) {}
