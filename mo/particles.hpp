/* 
 * File:   Particles.h
 * Author: morgan
 *
 * Created on October 16, 2014, 5:03 PM
 */

#ifndef MO_PARTICLES_H
#define	MO_PARTICLES_H

#include <glm/glm.hpp>
#include <vector>

#include "particle.hpp"

namespace mo {

    class Particles {
    public:
        Particles();
        template<class It>
        Particles(It begin, It end){
            particles_.assign(begin, end);
        }

        virtual ~Particles();

        using Parts = std::vector<Particle>;

        Parts::const_iterator begin() const {
            return particles_.begin();
        }

        Parts::const_iterator end() const {
            return particles_.end();
        }

        Parts::iterator begin() {
            return particles_.begin();
        }

        Parts::iterator end() {
            return particles_.end();
        }

        template<class It>
        void assign(It begin, It end) {
            particles_.assign(begin, end);
            valid = false;
        }

        void clear();
        void add(const Particle particle);
        Particle back();
        const Particle * data() const;
        unsigned int size() const;
        /**          
         * @return A unique identifier. 
         */
        unsigned int id() const;
        bool valid;

    private:
        Parts particles_;
        static unsigned int current_id;
        unsigned int id_;
    };

}

#endif	/* MO_PARTICLES_H */

