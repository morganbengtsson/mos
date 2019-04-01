#include <json.hpp>
#include <filesystem/path.h>
#include <mos/gfx/material.hpp>
#include <mos/util.hpp>
#include <mos/gfx/assets.hpp>
#include <iostream>

namespace mos {
namespace gfx {

using namespace nlohmann;

Material::Material(Shared_texture_2D albedo_map,
                   Shared_texture_2D normal_map,
                   Shared_texture_2D metallic_map,
                   Shared_texture_2D roughness_map,
                   Shared_texture_2D ambient_occlusion_map,
                   glm::vec3 albedo,
                   const float opacity,
                   const float transmission,
                   const float roughness,
                   const float metallic,
                   const float emission,
                   const float ambient_occlusion)
    : albedo(std::move(albedo)),
      opacity(opacity),
      emission(emission),
      transmission(transmission),
      roughness(roughness),
      metallic(metallic),
      ambient_occlusion(ambient_occlusion),
      albedo_map(std::move(albedo_map)),
      normal_map(std::move(normal_map)),
      metallic_map(std::move(metallic_map)),
      roughness_map(std::move(roughness_map)),
      ambient_occlusion_map(std::move(ambient_occlusion_map)) {}

Material::~Material() {}

Material::Material(const glm::vec3 &albedo,
                   const float opacity,
                   const float transmission,
                   const float roughness,
                   const float metallic,
                   const float emission,
                   const float ambient_occlusion)
    : albedo(albedo),
      opacity(opacity),
      emission(emission),
      transmission(transmission),
      roughness(roughness),
      metallic(metallic),
      ambient_occlusion(ambient_occlusion) {}

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
      normal_map = read_texture("normal_map", false);
      metallic_map = read_texture("metallic_map", false);
      roughness_map = read_texture("roughness_map", false);
      ambient_occlusion_map = read_texture("ambient_occlusion_map", false);

      albedo =  glm::vec3(value["albedo"][0], value["albedo"][1], value["albedo"][2]);
      opacity = value["opacity"];
      transmission = value["transmission"];
      std::cout << path << " " << "trans: " << transmission << std::endl;
      roughness = value["roughness"];
      metallic = value["metallic"];
      emission = value["emission"];
      ambient_occlusion = value["ambient_occlusion"];
    } else {
      throw std::runtime_error(path.substr(path.find_last_of(".")) +
          " file format is not supported.");
    }
  }
}
}
}
