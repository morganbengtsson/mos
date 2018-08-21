#pragma once
#include <memory>
#include <json.hpp>
#include <optional>
#include <mos/gfx/material.hpp>
#include <mos/gfx/mesh.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/models.hpp>
#include <mos/gfx/assets.hpp>

namespace mos {
namespace gfx {

class Assets;

/** Collection of properties for a renderable object. */
class Model final {
public:
  Model() = default;

  Model(Assets &assets, const nlohmann::json &json, const glm::mat4 &parent_transform = glm::mat4(1.0f));

  Model(const std::string &name, const SharedMesh &mesh, const glm::mat4 &transform = glm::mat4(1.0f),
        const Material &material = Material(glm::vec3(1.0f)));

  ~Model() = default;

  std::string name() const;

  glm::vec3 position() const;

  /** Set position. */
  void position(const glm::vec3 &position);

  /** Mesh shape. */
  SharedMesh mesh;

  /** Material. */
  Material material;

  /** Transform. */
  glm::mat4 transform;

  /** Children models. */
  Models models;
private:
  std::string name_;
};
}
}
