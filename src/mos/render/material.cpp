#include <mos/render/material.hpp>
#include <fstream>
#include <json.hpp>
#include <mos/util.hpp>
#include <filesystem/path.h>

namespace mos {
using namespace nlohmann;
Material::Material(const SharedTexture2D &albedo_map,
                   const SharedTexture2D &normal_map,
                   const SharedTexture2D &light_map,
                   const glm::vec3 &albedo,
                   const float opacity,
                   const float roughness,
                   const float metallic)
    : albedo_map(albedo_map), normal_map(normal_map), light_map(light_map),
       albedo(albedo), opacity(opacity),
      roughness(roughness),
      metallic(metallic) {
}

Material::Material(const std::string &path) {
  filesystem::path fpath = path;
  if (fpath.extension() == "material") {
    auto value = json::parse(mos::text(fpath.str()));
    std::string t = "";
    if (!value["albedo_map"].is_null()) {
      t = value["albedo_map"];
    }
    albedo_map = Texture2D::load(fpath.parent_path().str() + "/" + t);

    std::string n = "";
    if (!value["_map"].is_null()) {
      n = value["normal_map"];
    }
    normal_map = Texture2D::load(fpath.parent_path().str() + "/" + n);

    std::string l = "";
    if (!value["light_map"].is_null()) {
      l = value["light_map"];
    }
    light_map = Texture2D::load(fpath.parent_path().str() + "/" + l);

    albedo = glm::vec3(value["albedo"][0], value["albedo"][1], value["albedo"][2]);
    opacity = value["opacity"];
    roughness = value["roughness"];
    metallic = value["metallic"];

  } else {
    throw std::runtime_error(path.substr(path.find_last_of(".")) +
        " file format is not supported.");
  }
}

Material::~Material() {}

Material Material::load(const std::string &path) {
  filesystem::path fpath = path;
  if (fpath.is_directory()) {
    return Material();
  }
  return Material(path);
}
Material::Material(const glm::vec3 &albedo,
                   const float opacity,
                   const float roughness,
                   const float metallic)
    : albedo(albedo), opacity(opacity), roughness(roughness), metallic(metallic) {

}
}
