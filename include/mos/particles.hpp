/* 
 * File:   Particles.h
 * Author: morgan
 *
 * Created on October 16, 2014, 5:03 PM
 */

#ifndef MOS_PARTICLES_H
#define	MOS_PARTICLES_H

#include <vector>
#include "particle.hpp"

namespace mos {

/**
 * @brief The Particles class
 */
class Particles {
public:
    /**
     * @brief Particles constructor.
     */
    Particles();

    template<class It>
    /**
     * @brief Particles constructor
     * @param begin First particle iterator.
     * @param end Last particle iterator.
     */
    Particles(It begin, It end){
        particles_.assign(begin, end);
    }

    /**
     * @brief ~Particles destructor.
     */
    virtual ~Particles();

    /**
     * @brief Particles container.
     */
    using Parts = std::vector<Particle>;

    /**
     * @brief begin
     * @return Iterator to first particle.
     */
    Parts::const_iterator begin() const {
        return particles_.begin();
    }

    /**
     * @brief end
     * @return Iterator to last particle.
     */
    Parts::const_iterator end() const {
        return particles_.end();
    }

    /**
     * @brief begin
     * @return Iterator to first particle.
     */
    Parts::iterator begin() {
        return particles_.begin();
    }

    /**
     * @brief end
     * @return Iterator last particle.
     */
    Parts::iterator end() {
        return particles_.end();
    }


    template<class It>
    /**
     * @breif Assign new particles
     * @param begin Iterator to first particle.
     * @param end Iterator to last particle.
     */
    void assign(It begin, It end) {
        particles_.assign(begin, end);
        valid = false;
    }

    /**
     * @brief Clear the container.
     */
    void clear();

    /**
     * @brief Add a particle.
     * @param particle A particle.
     */
    void add(const Particle particle);

    /**
     * @brief back
     * @return Last particle.
     */
    Particle back();

    /**
     * @brief data
     * @return Raw data.
     */
    const Particle * data() const;

    /**
     * @brief Number of particles.
     * @return Number of particles.
     */
    unsigned int size() const;

    /**
     * @brief
     * @return A unique identifier.
     */
    unsigned int id() const;

    /**
     * @brief valid
     */
    bool valid;

private:
    Parts particles_;
    static unsigned int current_id;
    unsigned int id_;
};

}

#endif	/* MOS_PARTICLES_H */

