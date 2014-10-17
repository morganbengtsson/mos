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
    
    struct Particle {    
        Particle(const glm::vec3 & position = glm::vec3(0.0f)) :
        position(position){
        }
        ~Particle(){
        };

        glm::vec3 position;
        
    };
}
#endif	/* MO_PARTICLE_H */

