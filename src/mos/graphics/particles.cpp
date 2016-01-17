/* 
 * File:   Particles.cpp
 * Author: morgan
 * 
 * Created on October 16, 2014, 5:03 PM
 */

#include <mos/graphics/particles.hpp>

namespace mos {

    unsigned int Particles::current_id = 10000;
    
    Particles::Particles() : valid_(false) {
        id_ = current_id++;
    }

    Particles::~Particles() {
    }
    
    unsigned int Particles::id() const {
        return id_;
    }

    bool Particles::valid() const {
        return valid_;
    }

    void Particles::invalidate() {
        valid_ = false;
    }

    void Particles::add(const Particle particle) {
        particles_.push_back(particle);
        valid_ = false;
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

