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
               bool loop,
               bool playing)
    : position(position),
      velocity(velocity),
      pitch(pitch),
      gain(gain),
      loop(loop),
      playing(playing)
{
    static unsigned int current_id = 0;
    id_ = current_id++;
}

Source::Source(const Source &source){
    id_ = source.id();
    position = source.position;
    velocity = source.velocity;
    pitch = source.pitch;
    gain = source.gain;
    loop = source.loop;
    playing = source.playing;
}

Source::~Source() {
}

unsigned int Source::id() const{
    return id_;
}
}
