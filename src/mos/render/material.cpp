#include <mos/render/material.hpp>
#include <fstream>
#include <json.hpp>
#include <mos/util.hpp>
#include <filesystem/path.h>

namespace mos {
using namespace nlohmann;
Material::Material(const SharedTexture &diffuse_map,
                   const SharedTexture &normal_map,
                   const SharedTexture &light_map,
                   const SharedTexture &diffuse_environment_map,
                   const SharedTextureCube &specular_environment_map,
                   const glm::vec3 &ambient,
                   const glm::vec3 &diffuse,
                   const glm::vec3 &specular,
                   const float opacity,
                   const float specular_exponent)
    : diffuse_map(diffuse_map), normal_map(normal_map), light_map(light_map),
      diffuse_environment_map(diffuse_environment_map),
      specular_environment_map(specular_environment_map), ambient(ambient),
      diffuse(diffuse), specular(specular), opacity(opacity),
      specular_exponent(specular_exponent) {

  if (diffuse_environment_map) {
    diffuse_environment_map->wrap = mos::Texture::Wrap::CLAMP_TO_EDGE;
  }
  if (specular_environment_map) {
    specular_environment_map->wrap = mos::TextureCube::Wrap::CLAMP_TO_EDGE;

  }
}

Material::Material(const std::string &path) {
  filesystem::path fpath = path;
  if (fpath.extension() == "material") {
    /*std::ifstream is(path, std::ios::binary);
    is.read((char *)&ambient, sizeof(glm::vec3));
    is.read((char *)&diffuse, sizeof(glm::vec3));
    is.read((char *)&specular, sizeof(glm::vec3));
    is.read((char *)&opacity, sizeof(float));
    is.read((char *)&specular_exponent, sizeof(float));*/

    auto value = json::parse(mos::text(fpath.str()));
    std::string t = "";
    if (!value["diffuse_map"].is_null()) {
      t = value["diffuse_map"];
    }
    diffuse_map = Texture::load(fpath.parent_path().str() + "/" + t);

    std::string n = "";
    if (!value["normal_map"].is_null()) {
      n = value["normal_map"];
    }
    normal_map = Texture::load(fpath.parent_path().str() + "/" + n);

    std::string l = "";
    if (!value["light_map"].is_null()) {
      l = value["light_map"];
    }
    light_map = Texture::load(fpath.parent_path().str() + "/" + l);

    std::string diffusemap_file = "";
    if (!value["diffuse_environment_map"].is_null()) {
      diffusemap_file = value["diffuse_environment_map"];
    }
    diffuse_environment_map = Texture::load(fpath.parent_path().str() + "/" + diffusemap_file, false);

    std::string spec_file_pos_x = "";
    std::string spec_file_neg_x = "";
    std::string spec_file_pos_y = "";
    std::string spec_file_neg_y = "";
    std::string spec_file_pos_z = "";
    std::string spec_file_neg_z = "";
    if (!value["specular_environment_map_pos_x"].is_null()) {
      spec_file_pos_x = value["specular_environment_map_pos_x"];
    }
    if (!value["specular_environment_map_neg_x"].is_null()) {
      spec_file_neg_x = value["specular_environment_map_neg_x"];
    }
    if (!value["specular_environment_map_pos_y"].is_null()) {
      spec_file_pos_y = value["specular_environment_map_pos_y"];
    }
    if (!value["specular_environment_map_neg_y"].is_null()) {
      spec_file_neg_y = value["specular_environment_map_neg_y"];
    }
    if (!value["specular_environment_map_pos_z"].is_null()) {
      spec_file_pos_z = value["specular_environment_map_pos_z"];
    }
    if (!value["specular_environment_map_neg_z"].is_null()) {
      spec_file_neg_z = value["specular_environment_map_neg_z"];
    }
    specular_environment_map = TextureCube::load(fpath.parent_path().str() + "/" + spec_file_pos_x,
                                                 fpath.parent_path().str() + "/" + spec_file_neg_x,
                                                 fpath.parent_path().str() + "/" + spec_file_pos_y,
                                                 fpath.parent_path().str() + "/" + spec_file_neg_y,
                                                 fpath.parent_path().str() + "/" + spec_file_pos_z,
                                                 fpath.parent_path().str() + "/" + spec_file_neg_z, false);

    ambient = glm::vec3(value["ambient"][0], value["ambient"][1], value["ambient"][2]);
    diffuse = glm::vec3(value["diffuse"][0], value["diffuse"][1], value["diffuse"][2]);
    specular = glm::vec3(value["specular"][0], value["specular"][1], value["specular"][2]);
    opacity = value["opacity"];
    specular_exponent = value["specular_exponent"];

  } else {
    throw std::runtime_error(path.substr(path.find_last_of(".")) +
        " file format is not supported.");
  }
}

Material::~Material() {}

SharedMaterial Material::load(const std::string &path) {
  filesystem::path fpath = path;
  if (fpath.is_directory()) {
    return std::make_shared<Material>();
  }
  return std::make_shared<Material>(path);
}
Material::Material(const glm::vec3 &ambient,
                   const glm::vec3 &diffuse,
                   const glm::vec3 &specular,
                   const float opacity,
                   const float specular_exponent)
    : ambient(ambient), diffuse(diffuse), specular(specular), opacity(opacity), specular_exponent(specular_exponent) {

}
}
