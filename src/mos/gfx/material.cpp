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
                   Shared_texture_2D emission_map,
                   Shared_texture_2D normal_map,
                   Shared_texture_2D metallic_map,
                   Shared_texture_2D roughness_map,
                   Shared_texture_2D ambient_occlusion_map,
                   const glm::vec3 albedo,
                   const float alpha,
                   const float index_of_refraction,
                   const float transmission,
                   const float roughness,
                   const float metallic,
                   const glm::vec3 emission,
                   const float ambient_occlusion)
    : albedo(albedo),
      alpha(alpha),
      index_of_refraction(index_of_refraction),
      transmission(transmission),
      emission(emission),
      roughness(roughness),
      metallic(metallic),
      ambient_occlusion(ambient_occlusion),
      emission_map(std::move(emission_map)),
      albedo_map(std::move(albedo_map)),
      normal_map(std::move(normal_map)),
      metallic_map(std::move(metallic_map)),
      roughness_map(std::move(roughness_map)),
      ambient_occlusion_map(std::move(ambient_occlusion_map)) {}

Material::Material(const glm::vec3 &albedo,
                   const float alpha,
                   const float index_of_refraction,
                   const float transmission,
                   const float roughness,
                   const float metallic,
                   const glm::vec3 emission,
                   const float ambient_occlusion)
    : albedo(albedo),
      alpha(alpha),
      index_of_refraction(index_of_refraction),
      transmission(transmission),
      emission(emission),
      roughness(roughness),
      metallic(metallic),
      ambient_occlusion(ambient_occlusion) {}

Material::Material(Assets &assets, std::string &path) : Material() {
  if (!path.empty()) {
    filesystem::path fpath = path;
    if (fpath.extension() == "material") {
      auto value = json::parse(mos::text(assets.directory() + fpath.str()));

      //TODO: Move to texture class
      auto read_texture = [&](const std::string &name, const bool color_data = true) {
        std::string file_name{};
        if (!value[name].is_null()) {
		  std::string temp = value[name];
          file_name = temp;
        }
        if (file_name.empty()){
          return assets.texture("");
        }
        else {
          auto texture_json = json::parse(mos::text(assets.directory() + std::string(value[name])));
          std::string image_name = texture_json["image"];
          std::string filter = texture_json["filter"];
          std::string wrap = texture_json["wrap"];

          static const std::map<std::string, Texture_2D::Filter> filter_map{{"linear", Texture_2D::Filter::Linear}, {"closest", Texture_2D::Filter::Closest}};
          static const std::map<std::string, Texture_2D::Wrap> wrap_map{{"clamp", Texture_2D::Wrap::Clamp}, {"repeat", Texture_2D::Wrap::Repeat}};

          return assets.texture(image_name, color_data, true, filter_map.at(filter), wrap_map.at(wrap));
        }
      };

      albedo_map = read_texture("albedo_map");
      normal_map = read_texture("normal_map", false);
      emission_map = read_texture("emission_map", false);
      metallic_map = read_texture("metallic_map", false);
      roughness_map = read_texture("roughness_map", false);
      ambient_occlusion_map = read_texture("ambient_occlusion_map", false);

      albedo =  glm::vec3(value["albedo"][0], value["albedo"][1], value["albedo"][2]);
      index_of_refraction = value["index_of_refraction"];
      transmission = value["transmission"];
      alpha = value["alpha"];
      roughness = value["roughness"];
      metallic = value["metallic"];
      emission = glm::vec3(value["emission"][0], value["emission"][1], value["emission"][2]);
      ambient_occlusion = value["ambient_occlusion"];
    } else {
      throw std::runtime_error(path.substr(path.find_last_of('.')) +
          " file format is not supported.");
    }
  }
}
}
}
