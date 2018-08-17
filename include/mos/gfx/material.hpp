#pragma once
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <mos/gfx/texture_2d.hpp>

namespace mos {
namespace gfx {
class Material;
using SharedMaterial = std::shared_ptr<Material>;

/** Physically based material. */
class Material {
public:
  explicit Material(const SharedTexture2D &albedo_map = SharedTexture2D(),
                    const SharedTexture2D &emission_map = SharedTexture2D(),
                    const SharedTexture2D &normal_map = SharedTexture2D(),
                    const SharedTexture2D &metallic_map = SharedTexture2D(),
                    const SharedTexture2D &roughness_map = SharedTexture2D(),
                    const SharedTexture2D &ambient_occlusion_map = SharedTexture2D(),
                    const glm::vec3 &albedo = glm::vec3(1.0f),
                    const float opacity = 1.0f,
                    const float roughness = 0.0f,
                    const float metallic = 0.0f,
                    const glm::vec3 &emission = glm::vec3(0.0f),

                    const float ambient_occlusion = 1.0f);

  explicit Material(const glm::vec3 &albedo,
                    const float opacity = 1.0f,
                    const float roughness = 0.0f,
                    const float metallic = 0.0f,
                    const glm::vec3 &emission = glm::vec3(0.0f),
                    const float ambient_occlusion = 1.0f);

  virtual ~Material();

  glm::vec3 albedo;
  glm::vec3 emission;
  glm::vec3 factor;
  float opacity;
  float roughness;
  float metallic;
  float emission_strength;
  float ambient_occlusion;

  SharedTexture2D albedo_map;
  SharedTexture2D emission_map;
  SharedTexture2D normal_map;
  SharedTexture2D metallic_map;
  SharedTexture2D roughness_map;
  SharedTexture2D ambient_occlusion_map;
};
}
}

