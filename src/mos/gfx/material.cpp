#include <json.hpp>
#include <filesystem/path.h>
#include <mos/gfx/material.hpp>
#include <mos/util.hpp>
#include <mos/gfx/assets.hpp>

namespace mos {
namespace gfx {

using namespace nlohmann;

Material::Material(const Shared_texture_2D &albedo_map,
                   const Shared_texture_2D &emission_map,
                   const Shared_texture_2D &normal_map,
                   const Shared_texture_2D &metallic_map,
                   const Shared_texture_2D &roughness_map,
                   const Shared_texture_2D &ambient_occlusion_map,
                   const glm::vec3 &albedo,
                   const float opacity,
                   const float roughness,
                   const float metallic,
                   const glm::vec3 &emission,
                   const float ambient_occlusion,
                   const float strength)
    : albedo_map(albedo_map),
      emission_map(emission_map),
      normal_map(normal_map),
      metallic_map(metallic_map),
      roughness_map(roughness_map),
      ambient_occlusion_map(ambient_occlusion_map),
      strength(strength),
      albedo(albedo),
      opacity(opacity),
      emission(emission),
      roughness(roughness),
      metallic(metallic),
      ambient_occlusion(ambient_occlusion) {}

Material::~Material() {}

Material::Material(const glm::vec3 &albedo,
                   const float opacity,
                   const float roughness,
                   const float metallic,
                   const glm::vec3 &emission,
                   const float ambient_occlusion,
                   const float strength)
    : albedo(albedo), opacity(opacity), roughness(roughness), metallic(metallic), emission(emission),
      ambient_occlusion(ambient_occlusion), strength(strength) {

}
Material::Material(Assets &assets, std::string &path) : Material() {
  if (!path.empty()) {
    filesystem::path fpath = path;
    if (fpath.extension() == "material") {
      auto value = json::parse(mos::text(assets.directory() + fpath.str()));

      auto read_texture = [&](const std::string &name, const bool color_data = true) {
        std::string file_name = "";
        if (!value[name].is_null()) {
          file_name = value[name];
        }
        auto tex = file_name.empty() ? assets.texture("") : assets.texture(file_name, color_data);
        return tex;
      };

      albedo_map = read_texture("albedo_map");
      emission_map = read_texture("emission_map");
      normal_map = read_texture("normal_map", false);
      metallic_map = read_texture("metallic_map", false);
      roughness_map = read_texture("roughness_map", false);
      ambient_occlusion_map = read_texture("ambient_occlusion_map", false);

      albedo =  glm::vec3(value["albedo"][0], value["albedo"][1], value["albedo"][2]);
      strength = value["strength"];
      opacity = value["opacity"];
      roughness = value["roughness"];
      metallic = value["metallic"];
      emission = glm::vec3(value["emission"][0], value["emission"][1], value["emission"][2]);
      ambient_occlusion = value["ambient_occlusion"];
    } else {
      throw std::runtime_error(path.substr(path.find_last_of(".")) +
          " file format is not supported.");
    }
  }
}
}
}
