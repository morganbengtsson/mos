/* 
 * File:   Material.cpp
 * Author: morgan
 * 
 * Created on September 30, 2014, 9:35 PM
 */

#include "Material.h"

namespace mo {

    Material::Material(const glm::vec3 ambient_color,
            const glm::vec3 diffuse_color,
            const glm::vec3 specular_color,
            const float opacity,
            const float specular_exponent) :
    ambient(ambient_color),
    diffuse(diffuse_color),
    opacity(opacity),
    specular_exponent(specular_exponent) {
    }

    Material::~Material() {
    }

}