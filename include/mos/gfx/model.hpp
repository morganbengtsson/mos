#pragma once
#include <memory>
#include <json.hpp>
#include <optional>
#include <mos/gfx/material.hpp>
#include <mos/gfx/mesh.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/models.hpp>
#include <mos/gfx/assets.hpp>

namespace mos::gfx {

class Assets;
class Material;

/** Collection of properties for a renderable object. */
class Model final {
public:
  Model(const nlohmann::json &json, Assets &assets = *std::make_unique<Assets>(), const glm::mat4 &parent_transform = glm::mat4(1.0f));

  Model(std::string name,
        Shared_mesh mesh,
        glm::mat4 transform = glm::mat4(1.0f),
        Material material = Material{glm::vec3(1.0f)});

  Model() = default;

  auto name() const -> std::string;

  auto position() const -> glm::vec3;

  /** Set position. */
  auto position(const glm::vec3 &position) -> void;

  /** Get centroid position. */
  auto centroid() const -> glm::vec3;

  /** Get radious of bounding sphere */
  auto radius() const -> float;

  /** Set emission of material recursively. */
  auto emission(const glm::vec3 & emission) -> void;

  /** Set alpha of material recursively. */
  auto alpha(float alpha) -> void;

  /** Set ambient occlusion of material recursively. */
  auto ambient_occlusion(float ambient_occlusion) -> void;

  /** set index of refraction of material recursively. */
  auto index_of_refraction(float index_of_refraction) -> void;

  /** set transmission of material recursively. */
  auto transmission(float transmission) -> void;

  /** set roughness of material recursively. */
  auto roughness(float roughness) -> void;

  /** set metallic of material recursively. */
  auto metallic(float metallic) -> void;

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
