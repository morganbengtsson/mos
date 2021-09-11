#include <mos/gfx/gpu/material.hpp>

namespace mos::gfx::gpu {

Material::Material(const mos::gfx::Material &material)
    : albedo_(material.albedo), metallic_(material.metallic),
      roughness_(material.roughness), emission_(material.emission),
      ambient_occlusion_(material.ambient_occlusion), normal_(material.normal),
      alpha_(material.alpha), index_of_refraction_(material.index_of_refraction),
      transmission_(material.transmission) {}

const Material::Albedo& Material::albedo() const {
  return albedo_;
}

const Material::Metallic& Material::metallic() const {
  return metallic_;
}

const Material::Roughness& Material::roughness() const {
  return roughness_;
}

const Material::Emission& Material::emission() const {
  return emission_;
}

const Material::Ambient_occlusion& Material::ambient_occlusion() const {
  return ambient_occlusion_;
}

const Material::Normal& Material::normal() const {
  return normal_;
}

const float& Material::alpha() const {
  return alpha_;
}

const float& Material::index_of_refraction() const {
  return index_of_refraction_;
}

const float& Material::transmission() const {
  return transmission_;
}
} // namespace mos::gfx::gpu
