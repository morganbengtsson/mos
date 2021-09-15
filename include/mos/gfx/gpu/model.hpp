#pragma once

#include <vector>
#include <glm/gtx/matrix_decompose.hpp>
#include <mos/gfx/model.hpp>
#include <mos/gfx/renderer.hpp>

#include <mos/gfx/gpu/mesh.hpp>
#include <mos/gfx/gpu/material.hpp>

namespace mos::gfx::gpu {

class Model;

using Models = std::vector<mos::gfx::gpu::Model>;

class Model final {
  friend class mos::gfx::Renderer;
 private:
  explicit Model(mos::gfx::Model model): mesh(model.mesh), material(model.material), transform(model.transform) {
    for (auto model: model.models) {
      models.push_back(Model(model));
    }
  }
public:
  Model() = default;
  /** Loaded mesh **/
  Mesh mesh = Mesh();

  /** Material. */
  Material material;

  /** Transform. */
  glm::mat4 transform{0.0f};

  /** Children models. */
  Models models{};

  /** Get centroid position. */
  auto centroid() const -> glm::vec3 {
    return (transform * glm::translate(glm::mat4(1.0f), mesh.centroid()))[3];
  }

  /** Get radious of bounding sphere */
  auto radius() const -> float {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, scale, rotation, translation, skew, perspective);
    return mesh.radius() * glm::max(glm::max(scale.x, scale.y), scale.z);
  }

  auto position() const -> glm::vec3 {
    return glm::vec3(transform[3]);
  }
};
}
