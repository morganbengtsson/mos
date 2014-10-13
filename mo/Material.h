/* 
 * File:   Material.h
 * Author: morgan
 *
 * Created on September 30, 2014, 9:35 PM
 */

#ifndef MO_MATERIAL_H
#define	MO_MATERIAL_H

#include <glm/glm.hpp>

namespace mo {

    class Material {
    public:
        Material(const glm::vec3 ambient_color = glm::vec3(1.0f),
                const glm::vec3 diffuse_color = glm::vec3(1.0f),
                const glm::vec3 specular_color = glm::vec3(1.0f),
                const float opacity = 1.0f,
                const float specular_exponent = 0.0f);
        virtual ~Material();

        glm::vec3 ambient_color;
        glm::vec3 diffuse_color;
        glm::vec3 specular_color;
        float opacity;
        // Ns
        float specular_exponent;

    private:
    };
}

#endif	/* MATERIAL_H */

