/* 
 * File:   Light.cpp
 * Author: morgan
 * 
 * Created on October 29, 2014, 10:10 PM
 */

#include "Light.h"

namespace mo {

    Light::Light(const glm::vec3 position, 
              const glm::vec3 diffuse_color, 
              const glm::vec3 specular_color):
    position(position), 
    diffuse_color(diffuse_color), 
    specular_color(specular_color)
    {
    }

    Light::~Light() {
    }

}