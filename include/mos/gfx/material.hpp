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
  explicit Material(Shared_texture_2D albedo_map = Shared_texture_2D(),
                    Shared_texture_2D normal_map = Shared_texture_2D(),
                    Shared_texture_2D metallic_map = Shared_texture_2D(),
                    Shared_texture_2D roughness_map = Shared_texture_2D(),
                    Shared_texture_2D ambient_occlusion_map = Shared_texture_2D(),
                    glm::vec3 albedo = glm::vec3(1.0f),
                    float opacity = 1.0f,
                    float transmission = 0.0f,
                    float roughness = 0.0f,
                    float metallic = 0.0f,
                    float emission = 0.0f,
                    float ambient_occlusion = 1.0f);

  explicit Material(const glm::vec3 &albedo,
                    float opacity = 1.0f,
                    float transmission = 0.0f,
                    float roughness = 0.0f,
                    float metallic = 0.0f,
                    float emission = 0.0f,
                    float ambient_occlusion = 1.0f);

  explicit Material(Assets &assets, std::string &path);

  glm::vec3 albedo;
  float opacity;
  float emission;
  float transmission;
  float roughness;
  float metallic;
  float ambient_occlusion;

  Shared_texture_2D albedo_map;
  Shared_texture_2D normal_map;
  Shared_texture_2D metallic_map;
  Shared_texture_2D roughness_map;
  Shared_texture_2D ambient_occlusion_map;
};
}
}

