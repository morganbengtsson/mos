/* 
 * File:   Particles.cpp
 * Author: morgan
 * 
 * Created on October 16, 2014, 5:03 PM
 */

#include "particles.hpp"

namespace mo {

    unsigned int Particles::current_id = 10000;
    
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

    const Particle * Particles::data() const {
        return particles_.data();
    }

    unsigned int Particles::size() const {
        return particles_.size();
    }

    void Particles::clear() {
        particles_.clear();
    }


}

