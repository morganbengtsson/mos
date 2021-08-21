#pragma once

#include <mos/gfx/mesh.hpp>
#include <mos/gfx/model.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace mos::gfx {

class Renderer {
public:
  #include "../../src/mos/gfx/renderer/mesh.hpp"
  #include "../../src/mos/gfx/renderer/model.hpp"
protected:
  Mesh convert_mesh(const mos::gfx::Mesh &mesh) const {
    return Mesh(mesh);
  }
  Model convert_model(const mos::gfx::Model &model) const {
    return Model(model);
  }
};
}
