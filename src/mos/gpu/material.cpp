#include <mos/gpu/material.hpp>

namespace mos::gpu {

Material::Material(const mos::gfx::Material &material)
    : albedo_(material.albedo), metallic_(material.metallic),
      roughness_(material.roughness), emission_(material.emission),
      ambient_occlusion_(material.ambient_occlusion), normal_(material.normal),
      alpha_(material.alpha), index_of_refraction_(material.index_of_refraction),
      transmission_(material.transmission) {}

auto Material::albedo() const -> const decltype(albedo_)& {
  return albedo_;
}

auto Material::metallic() const -> const decltype (metallic_)& {
  return metallic_;
}

auto Material::roughness() const -> const decltype(roughness_)& {
  return roughness_;
}

auto Material::emission() const -> const decltype(emission_)& {
  return emission_;
}

auto Material::ambient_occlusion() const -> const decltype(ambient_occlusion_)& {
  return ambient_occlusion_;
}

auto Material::normal() const -> const decltype(normal_)& {
  return normal_;
}

auto Material::alpha() const -> const decltype(alpha_)& {
  return alpha_;
}

auto Material::index_of_refraction() const -> const decltype(index_of_refraction_)& {
  return index_of_refraction_;
}

auto Material::transmission() const -> const decltype(transmission_)& {
  return transmission_;
}
} // namespace mos::gpu
