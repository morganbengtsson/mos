#pragma once
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <mos/gfx/texture_2d.hpp>

namespace mos {
namespace gfx {

class Assets;

/** Physically based material. */
class Material final {
public:
  explicit Material(const SharedTexture2D &albedo_map = SharedTexture2D(),
                    const SharedTexture2D &emission_map = SharedTexture2D(),
                    const SharedTexture2D &normal_map = SharedTexture2D(),
                    const SharedTexture2D &metallic_map = SharedTexture2D(),
                    const SharedTexture2D &roughness_map = SharedTexture2D(),
                    const SharedTexture2D &ambient_occlusion_map = SharedTexture2D(),
                    const glm::vec3 &albedo = glm::vec3(1.0f),
                    float opacity = 1.0f,
                    float roughness = 0.0f,
                    float metallic = 0.0f,
                    const glm::vec3 &emission = glm::vec3(0.0f),
                    float ambient_occlusion = 1.0f);

  explicit Material(const glm::vec3 &albedo,
                    float opacity = 1.0f,
                    float roughness = 0.0f,
                    float metallic = 0.0f,
                    const glm::vec3 &emission = glm::vec3(0.0f),
                    float ambient_occlusion = 1.0f);

  explicit Material(Assets &assets, std::string &path);

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

