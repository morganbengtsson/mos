#include <mos/render/material.hpp>
#include <fstream>
#include <json.hpp>
#include <mos/util.hpp>
#include <filesystem/path.h>

namespace mos {
using namespace nlohmann;
Material::Material(const SharedTexture2D &diffuse_map,
                   const SharedTexture2D &emission_map,
                   const SharedTexture2D &normal_map,
                   const SharedTexture2D &light_map,
                   const glm::vec3 &ambient,
                   const glm::vec3 &diffuse,
                   const glm::vec3 &specular,
                   const glm::vec3 &emission,
                   const float opacity,
                   const float shininess)
    : diffuse_map(diffuse_map), emission_map(emission_map), normal_map(normal_map), light_map(light_map),
       ambient(ambient), diffuse(diffuse), specular(specular), emission(emission), opacity(opacity),
      shininess(shininess) {
}

Material::Material(const std::string &path) {
  filesystem::path fpath = path;
  if (fpath.extension() == "material") {
    auto value = json::parse(mos::text(fpath.str()));
    std::string t = "";
    if (!value["diffuse_map"].is_null()) {
      t = value["diffuse_map"];
    }
    diffuse_map = Texture2D::load(fpath.parent_path().str() + "/" + t);

    std::string e = "";
    if (!value["emission_map"].is_null()) {
      e = value["emission_map"];
    }
    emission_map = Texture2D::load(fpath.parent_path().str() + "/" + e);

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

    ambient = glm::vec3(value["ambient"][0], value["ambient"][1], value["ambient"][2]);
    diffuse = glm::vec3(value["diffuse"][0], value["diffuse"][1], value["diffuse"][2]);
    specular = glm::vec3(value["specular"][0], value["specular"][1], value["specular"][2]);
    emission = glm::vec3(value["emission"][0], value["emission"][1], value["emission"][2]);
    opacity = value["opacity"];
    shininess = value["shininess"];

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
Material::Material(const glm::vec3 &ambient,
                   const glm::vec3 &diffuse,
                   const glm::vec3 &specular,
                   const glm::vec3 &emission,
                   const float opacity,
                   const float shininess)
    : ambient(ambient), diffuse(diffuse), specular(specular), opacity(opacity), shininess(shininess) {

}
}
