/* 
 * File:   Particles.cpp
 * Author: morgan
 * 
 * Created on October 16, 2014, 5:03 PM
 */

#include "Particles.h"

namespace mo {

    unsigned int Particles::current_id = 100;
    
    Particles::Particles() : valid(true) {
        id_ = current_id++;
    }

    Particles::~Particles() {
    }
    
    unsigned int Particles::id() const {
        return id_;
    }

    void Particles::add(const Particle particle) {
        particles_.push_back(particle);
        valid = false;
    }

    Particle Particles::back() {
        return particles_.back();
    }

    void Particles::clear() {
        particles_.clear();
    }


}

