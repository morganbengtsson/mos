#pragma once
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <mos/gfx/texture_2d.hpp>

namespace mos::gfx {

class Assets;

/** Physically based material. */
class Material final {
public:
  template<class T>
  struct Slot {
    Slot(const T &value) : value(value) {}
    Slot(const Shared_texture_2D &texture) : texture(texture) {}
    Slot(const T &value, const Shared_texture_2D &texture) : value(value), texture(texture) {}
    T value = T();
    Shared_texture_2D texture = Shared_texture_2D();
  };

  struct Normal {
    Shared_texture_2D texture;
  };

  static auto load(Assets &assets, const std::string &path) -> Material;

  using Albedo = Slot<glm::vec3>;
  using Roughness = Slot<float>;
  using Metallic = Slot<float>;
  using Emission = Slot<glm::vec3>;
  using Ambient_occlusion = Slot<float>;

  Albedo albedo{glm::vec3(0.0f)};
  Metallic metallic{0.0f};
  Roughness roughness{1.0f};
  Emission emission{glm::vec3(0.0f)};
  Ambient_occlusion ambient_occlusion{1.0f};
  Normal normal{};

  float alpha{1.0f};
  float index_of_refraction{1.5f};
  float transmission{0.0f};
};
}
