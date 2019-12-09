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
  template<class T>
  struct Slot {
    T value;
    Shared_texture_2D texture;
  };

  struct Normal {
    Shared_texture_2D texture;
  };

  using Albedo = Slot<glm::vec3>;
  using Emission = Slot<glm::vec3>;
  using Roughness = Slot<float>;
  using Metallic = Slot<float>;
  using Ambient_occlusion = Slot<float>;

  explicit Material(Shared_texture_2D albedo_map = Shared_texture_2D(),
                    Shared_texture_2D emission_map = Shared_texture_2D(),
                    Shared_texture_2D normal_map = Shared_texture_2D(),
                    Shared_texture_2D metallic_map = Shared_texture_2D(),
                    Shared_texture_2D roughness_map = Shared_texture_2D(),
                    Shared_texture_2D ambient_occlusion_map = Shared_texture_2D(),
                    glm::vec3 albedo = glm::vec3(1.0f),
                    float alpha = 1.0f,
                    float index_of_refraction = 1.0f,
                    float transmission = 0.0f,
                    float roughness = 1.0f,
                    float metallic = 0.0f,
                    glm::vec3 emission = glm::vec3(0.0f),
                    float ambient_occlusion = 1.0f);

  explicit Material(const glm::vec3 &albedo,
                    float alpha = 1.0f,
                    float index_of_refraction = 1.0f,
                    float transmission = 0.0f,
                    float roughness = 1.0f,
                    float metallic = 0.0f,
                    glm::vec3 emission = glm::vec3(0.0f),
                    float ambient_occlusion = 1.0f);

  explicit Material(Assets &assets, std::string &path);

  Albedo albedo{glm::vec3(0.0f), Shared_texture_2D()};
  Metallic metallic{0.0f, Shared_texture_2D()};
  Roughness roughness{1.0f, Shared_texture_2D()};
  Emission emission{glm::vec3(0.0f), Shared_texture_2D()};
  Ambient_occlusion ambient_occlusion{1.0f, Shared_texture_2D()};
  Normal normal{Shared_texture_2D()};

  float alpha{1.0f};
  float index_of_refraction{1.5f};
  float transmission{0.0f};
};
}
}

