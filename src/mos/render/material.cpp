#include <mos/render/material.hpp>
#include <fstream>
#include <json.hpp>
#include <mos/util.hpp>
#include <string>
namespace mos {
using namespace nlohmann;
Material::Material(const SharedTexture &texture,
                   const SharedTexture &normalmap,
                   const SharedTexture &diffusemap,
                   const SharedTexture &specularmap,
                   const glm::vec3 &ambient,
                   const glm::vec3 &diffuse,
                   const glm::vec3 &specular,
                   const float opacity,
                   const float specular_exponent)
    : texture(texture), normalmap(normalmap), diffusemap(diffusemap), specularmap(specularmap), ambient(ambient),
      diffuse(diffuse), specular(specular), opacity(opacity),
      specular_exponent(specular_exponent) {}

Material::Material(const std::string &directory, const std::string &path) {
  if (path.substr(path.find_last_of(".") + 1) == "material") {
    /*std::ifstream is(path, std::ios::binary);
    is.read((char *)&ambient, sizeof(glm::vec3));
    is.read((char *)&diffuse, sizeof(glm::vec3));
    is.read((char *)&specular, sizeof(glm::vec3));
    is.read((char *)&opacity, sizeof(float));
    is.read((char *)&specular_exponent, sizeof(float));*/

    auto value = json::parse(mos::text(directory + path));
    std::string t = "";
    if (!value["texture"].is_null()) {
      t = value["texture"];
    }
    texture = Texture::load(directory + t);

    std::string n = "";
    if (!value["normalmap"].is_null()) {
      n = value["normalmap"];
    }
    normalmap = Texture::load(directory + n);

    std::string diffusemap_file = "";
    if (!value["diffusemap"].is_null()) {
      diffusemap_file = value["diffusemap"];
    }
    diffusemap = Texture::load(directory + diffusemap_file);

    std::string specularmap_file = "";
    if (!value["specularmap"].is_null()) {
      specularmap_file = value["specularmap"];
    }
    specularmap = Texture::load(directory + specularmap_file);

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

SharedMaterial Material::load(const std::string &directory, const std::string &path) {
  if (path.empty() || (path.back() == '/')) {
    return std::make_shared<Material>();
  }
  return std::make_shared<Material>(directory, path);
}
Material::Material(const glm::vec3 &ambient,
                   const glm::vec3 &diffuse,
                   const glm::vec3 &specular,
                   const float opacity,
                   const float specular_exponent)
    : ambient(ambient), diffuse(diffuse), specular(specular), opacity(opacity), specular_exponent(specular_exponent) {

}
}
