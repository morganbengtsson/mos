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

    /*!
     * \brief The Material class
     *
     * Aspects of a material supported by the renderer. Different from
     * texture.
     *
     */
    class Material {
    public:
        /*!
         * \brief Material
         * \param ambient
         * \param diffuse
         * \param specular
         * \param opacity
         * \param specular_exponent
         */
        Material(const glm::vec3 ambient = glm::vec3(1.0f),
                const glm::vec3 diffuse = glm::vec3(1.0f),
                const glm::vec3 specular = glm::vec3(1.0f),
                const float opacity = 1.0f,
                const float specular_exponent = 0.0f);
        virtual ~Material();

        /*!
         * \brief ambient color.
         */
        glm::vec3 ambient;

        /*!
         * \brief diffuse color.
         */
        glm::vec3 diffuse;

        /*!
         * \brief specular color.
         */
        glm::vec3 specular;

        /*!
         * \brief opacity
         */
        float opacity;

        /*!
         * \brief specular_exponent
         *
         * In some equations also calld Ns.
         */
        float specular_exponent;

    private:
    };
}

#endif	/* MATERIAL_H */

