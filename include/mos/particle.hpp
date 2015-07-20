#ifndef MOS_PARTICLE_H
#define	MOS_PARTICLE_H

#include <glm/glm.hpp>

namespace mos {

/**
 * @brief The Particle struct
 */
struct Particle {

    /**
     * @brief Particle constructor.
     * @param position
     * @param color
     * @param size Diameter.
     */
    explicit Particle(const glm::vec3 & position = glm::vec3(0.0f),
                      const glm::vec4 & color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                      const float size = 10.0f) :
        position(position),
        color(color),
        size(size){
    }

    /**
    * @brief Particle destructor.
    */
    ~Particle(){
    }

    /**
     * @brief Position of particle.
     */
    glm::vec3 position;

    /**
     * @brief Color of particle.
     */
    glm::vec4 color;

    /**
     * @brief Size of particle (diameter).
     */
    float size;
};
}
#endif	/* MOS_PARTICLE_H */

