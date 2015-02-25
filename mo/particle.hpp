/* 
 * File:   Particle.h
 * Author: morgan
 *
 * Created on October 16, 2014, 4:54 PM
 */

#ifndef MO_PARTICLE_H
#define	MO_PARTICLE_H

#include <glm/glm.hpp>

namespace mo {
    
/*!
 * \brief The Particle struct
 */
struct Particle {

    /*!
     * \brief Particle constructor
     * \param position
     * \param color
     */
    Particle(const glm::vec3 & position = glm::vec3(0.0f),
             const glm::vec4 & color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
             const float size = 1.0f) :
    position(position),
    color(color),
    size(size){
    }
    ~Particle(){
    }

    glm::vec3 position;
    glm::vec4 color;
    float size;

};
}
#endif	/* MO_PARTICLE_H */

