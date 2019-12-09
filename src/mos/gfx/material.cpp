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
    : albedo{albedo, albedo_map},
      metallic{metallic, metallic_map},
      roughness{roughness, roughness_map},
      emission{emission, emission_map},
      ambient_occlusion{ambient_occlusion, ambient_occlusion_map},
      normal{normal_map},
      alpha(alpha),
      index_of_refraction(index_of_refraction),
      transmission(transmission){}

Material::Material(const glm::vec3 &albedo,
                   const float alpha,
                   const float index_of_refraction,
                   const float transmission,
                   const float roughness,
                   const float metallic,
                   const glm::vec3 emission,
                   const float ambient_occlusion)
    : albedo{albedo, Shared_texture_2D()},
      metallic{metallic, Shared_texture_2D()},
      roughness{roughness, Shared_texture_2D()},
      emission{emission, Shared_texture_2D()},
      ambient_occlusion{ambient_occlusion, Shared_texture_2D()},
      alpha(alpha),
      index_of_refraction(index_of_refraction),
      transmission(transmission){}

Material::Material(Assets &assets, std::string &path) : Material() {
  if (!path.empty()) {
    filesystem::path fpath = path;
    if (fpath.extension() == "material") {
      auto value = json::parse(mos::text(assets.directory() + fpath.str()));

      auto read_texture = [&](const std::string &name, const bool color_data = true) {
        std::string file_name{};
        if (!value[name]["texture"].is_null()) {
                  std::string temp = value[name]["texture"];
          file_name = temp;
        }
        if (file_name.empty()){
          return assets.texture("");
        }
        else {
          return assets.texture(value[name]["texture"].get<std::string>(), color_data, true);
        }
      };

      auto albedo_map = read_texture("albedo");
      auto albedo_value =  glm::vec3(value["albedo"]["value"][0], value["albedo"]["value"][1], value["albedo"]["value"][2]);
      albedo = {albedo_value, albedo_map};

      auto normal_map = read_texture("normal", false);
      normal = {normal_map};

      auto emission_map = read_texture("emission", false);
      auto emission_value = glm::vec3(value["emission"]["value"][0], value["emission"]["value"][1], value["emission"]["value"][2]);
      emission = {emission_value, emission_map};

      auto metallic_map = read_texture("metallic", false);
      auto metallic_value = value["metallic"]["value"];
      metallic = {metallic_value, metallic_map};

      auto roughness_map = read_texture("roughness", false);
      auto roughness_value = value["roughness"]["value"];
      roughness = {roughness_value, roughness_map};

      auto ambient_occlusion_map = read_texture("ambient_occlusion", false);
      auto ambient_occlusion_value = value["ambient_occlusion"]["value"];
      ambient_occlusion = {ambient_occlusion_value, ambient_occlusion_map};

      index_of_refraction = value["index_of_refraction"];
      transmission = value["transmission"];
      alpha = value["alpha"];
    } else {
      throw std::runtime_error(path.substr(path.find_last_of('.')) +
          " file format is not supported.");
    }
  }
}
}
}
