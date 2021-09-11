#pragma once

#include <mos/gfx/mesh.hpp>
#include <mos/gfx/model.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <mos/gfx/gpu/mesh.hpp>
#include <mos/gfx/gpu/model.hpp>

namespace mos::gfx {

class Renderer {
protected:
  gpu::Mesh convert_mesh(const mos::gfx::Mesh &mesh) const {
    return gpu::Mesh(mesh);
  }
  gpu::Model convert_model(const mos::gfx::Model &model) const {
    return gpu::Model(model);
  }
};
}
