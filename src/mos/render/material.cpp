#include <mos/render/material.hpp>
#include <fstream>

namespace mos {
  Material::Material(const std::string & path) {
    if (path.substr(path.find_last_of(".") + 1) == "material") {
      std::ifstream is(path, std::ios::binary);

      is.read((char *)&ambient, sizeof(glm::vec3));
      is.read((char *)&diffuse, sizeof(glm::vec3));
      is.read((char *)&specular, sizeof(glm::vec3));
      is.read((char *)&opacity, sizeof(float));
      is.read((char *)&specular_exponent, sizeof(float));
    } else {
      throw std::runtime_error(path.substr(path.find_last_of(".")) +
      " file format is not supported.");
    }
  }

  Material::Material(const glm::vec3 &ambient, const glm::vec3 &diffuse,
                   const glm::vec3 &specular, const float opacity,
                   const float specular_exponent)
    : ambient(ambient), diffuse(diffuse), specular(specular), opacity(opacity),
      specular_exponent(specular_exponent) {}

Material::~Material() {}
}
