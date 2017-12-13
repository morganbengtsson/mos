#include <mos/render/material.hpp>
#include <fstream>
#include <json.hpp>
#include <mos/util.hpp>
#include <filesystem/path.h>

namespace mos {
using namespace nlohmann;
Material::Material(const SharedTexture2D &albedo_map,
                   const SharedTexture2D &normal_map,
                   const SharedTexture2D &metallic_map,
                   const SharedTexture2D &roughness_map,
                   const SharedTexture2D &ambient_occlusion_map,
                   const SharedTexture2D &light_map,
                   const glm::vec3 &albedo,
                   const float opacity,
                   const float roughness,
                   const float metallic)
    : albedo_map(albedo_map), normal_map(normal_map), metallic_map(metallic_map), roughness_map(roughness_map),
      ambient_occlusion_map(ambient_occlusion_map), light_map(light_map),
      albedo(albedo), opacity(opacity),
      roughness(roughness),
      metallic(metallic) {
}

Material::~Material() {}

Material::Material(const glm::vec3 &albedo,
                   const float opacity,
                   const float roughness,
                   const float metallic)
    : albedo(albedo), opacity(opacity), roughness(roughness), metallic(metallic) {

}
}
