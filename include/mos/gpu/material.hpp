#pragma once

#include <mos/gfx/material.hpp>
#include <mos/gpu/resource.hpp>
#include <mos/gpu/texture_2d.hpp>

namespace mos::gpu {

/* Material living on the GPU */
class Material {
  friend class mos::gfx::Renderer;
  friend class Model;
public:
  template<class T>
  struct Pair {
    explicit Pair(const mos::gfx::Material::Slot<T> slot): value(slot.value) , texture(slot.texture){}
    explicit Pair(const T &value) : value(value) {}
    explicit Pair(const Texture_2D &texture) : texture(texture) {}
    Pair(const T &value, const Texture_2D &texture) : value(value), texture(texture) {}
    T value = T();
    Texture_2D texture = Texture_2D();
  };

  struct Normal {
    Normal() = default;
    Normal(const mos::gfx::Material::Normal &normal) : texture(normal.texture){}
    Texture_2D texture = Texture_2D();
  };

  using Albedo = Pair<glm::vec3>;
  using Roughness = Pair<float>;
  using Metallic = Pair<float>;
  using Emission = Pair<glm::vec3>;
  using Ambient_occlusion = Pair<float>;

  Material() = default;

  auto albedo() const -> const Albedo&;
  auto metallic() const -> const Metallic&;
  auto roughness() const -> const Roughness&;
  auto emission() const -> const Emission&;
  auto ambient_occlusion() const -> const Ambient_occlusion&;
  auto normal() const -> const Normal&;

  auto alpha() const -> const float&;
  auto index_of_refraction() const -> const float&;
  auto transmission() const -> const float&;

private:
  Material(const mos::gfx::Material &material);

  Albedo albedo_{glm::vec3(0.0f)};
  Metallic metallic_{0.0f};
  Roughness roughness_{1.0f};
  Emission emission_{glm::vec3(0.0f)};
  Ambient_occlusion ambient_occlusion_{1.0f};
  Normal normal_{};

  float alpha_{1.0f};
  float index_of_refraction_{1.5f};
  float transmission_{0.0f};
};
}

