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
class Material;

/** Collection of properties for a renderable object. */
class Model final {
public:
  Model(const nlohmann::json &json, Assets &assets = *std::make_unique<Assets>(), const glm::mat4 &parent_transform = glm::mat4(1.0f));

  Model(std::string name,
        Shared_mesh mesh,
        glm::mat4 transform = glm::mat4(1.0f),
        Material material = Material(glm::vec3(1.0f)));

  Model() = default;

  std::string name() const;

  glm::vec3 position() const;

  /** Set position. */
  void position(const glm::vec3 &position);

  /** Get centroid position. */
  glm::vec3 centroid() const;

  /** Get radious of bounding sphere */
  float radius() const;

  /** Set emission of material recursively. */
  void emission(const glm::vec3 & emission);

  /** Set alpha of material recursively. */
  void alpha(const float alpha);

  /** Set ambient occlusion of material recursively. */
  void ambient_occlusion(float ambient_occlusion);

  /** set index of refraction of material recursively. */
  void index_of_refraction(float index_of_refraction);

  /** set transmission of material recursively. */
  void transmission(float transmission);

  /** set roughness of material recursively. */
  void roughness(float roughness);

  /** set metallic of material recursively. */
  void metallic(float metallic);

  /** Mesh shape. */
  Shared_mesh mesh;

  /** Material. */
  Material material;

  /** Transform. */
  glm::mat4 transform{0.0f};

  /** Children models. */
  Models models;
private:
  std::string name_;
};
}
}
