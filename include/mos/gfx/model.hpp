#pragma once
#include <memory>
#include <json.hpp>
#include <optional>
#include <mos/gfx/material.hpp>
#include <mos/gfx/mesh.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/models.hpp>

namespace mos {
namespace gfx {

/** Collection of properties to render an object. */
class Model final {
public:
  Model();

  Model(const std::string &name, const SharedMesh &mesh,
        const glm::mat4 &transform = glm::mat4(1.0f),
        const Material &material = Material(
            Material(glm::vec3(1.0f))));

  ~Model();

  std::string name() const;

  glm::vec3 position() const;

  /** Set position. */
  void position(const glm::vec3 &position);

  /** A mesh shape. */
  SharedMesh mesh;

  /** A material. */
  Material material;

  /** A transform. */
  glm::mat4 transform;

  /** Children models. */
  Models models;
private:
  std::string name_;
};
}
}
