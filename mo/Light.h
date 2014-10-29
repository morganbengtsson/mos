/* 
 * File:   Light.h
 * Author: morgan
 *
 * Created on October 29, 2014, 10:10 PM
 */

#ifndef MO_LIGHT_H
#define	MO_LIGHT_H

#include <glm/glm.hpp>

namespace mo {

    class Light {
    public:
        Light(const glm::vec3 position = glm::vec3(0.0f), 
              const glm::vec3 diffuse_color = glm::vec3(1.0f), 
              const glm::vec3 specular_color = glm::vec3(1.0f),
              const float specular_exponent = 10.0f);
        virtual ~Light();
        glm::vec3 position;
        glm::vec3 diffuse_color;
        glm::vec3 specular_color;
        float specular_exponent;
    private:

    };
}

#endif	/* MO_LIGHT_H */

