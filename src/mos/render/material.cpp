#include <mos/render/material.hpp>
#include <fstream>

namespace mos {
  Material::Material(const glm::vec3 &ambient, const glm::vec3 &diffuse,
                   const glm::vec3 &specular, const float opacity,
                   const float specular_exponent)
    : ambient(ambient), diffuse(diffuse), specular(specular), opacity(opacity),
      specular_exponent(specular_exponent) {}

Material::~Material() {}
}
