/* 
 * File:   Source.cpp
 * Author: morgan
 * 
 * Created on July 28, 2014, 3:47 PM
 */

#include "source.hpp"

namespace mo {

Source::Source(const glm::vec3 position,
               const glm::vec3 velocity,
               const float pitch,
               const float gain,
               const bool loop,
               const bool playing,
               const float occlusion_factor,
               const bool outside)
    : position(position),
      velocity(velocity),
      pitch(pitch),
      gain(gain),
      loop(loop),
      playing(playing),
      occlusion_factor(occlusion_factor),
      outside(outside) {
    static unsigned int current_id = 0;
    id_ = current_id++;
}


Source::~Source() {
}

unsigned int Source::id() const {
    return id_;
}
}
