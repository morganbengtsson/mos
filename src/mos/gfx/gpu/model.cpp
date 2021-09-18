#include <mos/gfx/gpu/model.hpp>

namespace mos::gfx::gpu {

Model::Model(mos::gfx::Model model)
    : mesh(model.mesh), material(model.material), transform(model.transform) {
  for (auto& model : model.models) {
    models.push_back(Model(model));
  }
}

auto Model::centroid() const -> glm::vec3{
  return (transform * glm::translate(glm::mat4(1.0f), mesh.centroid()))[3];
}

auto Model::radius() const -> float {
  glm::vec3 scale;
  glm::quat rotation;
  glm::vec3 translation;
  glm::vec3 skew;
  glm::vec4 perspective;
  glm::decompose(transform, scale, rotation, translation, skew, perspective);
  return mesh.radius() * glm::max(glm::max(scale.x, scale.y), scale.z);
}

auto Model::position() const -> glm::vec3 {
  return glm::vec3(transform[3]);
}
}
