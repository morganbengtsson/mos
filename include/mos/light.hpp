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

     /*!
     * @brief The Light class
     *
     * A class describing different aspects of a light source. Used together with the renderer.
     */
    class Light {
    public:
        /*!
         * @brief Light
         * @param position
         * @param diffuse_color
         * @param specular_color
         */
        explicit Light(const glm::vec3 & position = glm::vec3(0.0f),
              const glm::vec3 & diffuse_color = glm::vec3(0.0f),
              const glm::vec3 & specular_color = glm::vec3(0.0f));
        virtual ~Light();

        /*!
         * @brief position
         */
        glm::vec3 position;

        /*!
         * @brief diffuse_color
         */
        glm::vec3 diffuse_color;

        /*!
         * @brief specular_color
         */
        glm::vec3 specular_color;        
    };
}

#endif	/* MO_LIGHT_H */

