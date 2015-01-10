/* 
 * File:   Source.cpp
 * Author: morgan
 * 
 * Created on July 28, 2014, 3:47 PM
 */

#include "source.hpp"

namespace mo {

Source::Source(const std::shared_ptr<Sound> sound, const glm::vec3 position, const bool loop) :
    sound(sound),
    position(position),
    loop(loop){
    static unsigned int current_id = 0;
    id_ = current_id++;
}

Source::~Source() {
}

unsigned int Source::id() const{
    return id_;
}
}
