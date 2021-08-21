#pragma once

#include <vector>
#include <glm/gtx/matrix_decompose.hpp>
#include <mos/gfx/model.hpp>
#include <mos/gfx/material.hpp>

#include "mesh.hpp"

class Model final {
  friend class Renderer;
  friend class Mesh;
private:
  Model(mos::gfx::Model model): mesh(model.mesh ? Mesh(*model.mesh) : Mesh()), material(model.material), transform(model.transform) {
    for (auto model: model.models) {
      models.push_back(Model(model));
    }
  }
public:
  Model() = default;
  /** Loaded mesh **/
  Mesh mesh = Mesh();

  /** Material. */
  Material material{};

  /** Transform. */
  glm::mat4 transform{0.0f};

  /** Children models. */
  std::vector<Model> models{};

  /** Get centroid position. */
  auto centroid() const -> glm::vec3 {
    return (transform * glm::translate(glm::mat4(1.0f), mesh.centroid))[3];
  }

  /** Get radious of bounding sphere */
  auto radius() const -> float {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, scale, rotation, translation, skew, perspective);
    return mesh.radius * glm::max(glm::max(scale.x, scale.y), scale.z);
  }

  auto position() const -> glm::vec3 {
    return glm::vec3(transform[3]);
  }
};
