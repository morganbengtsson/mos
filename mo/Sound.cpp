/* 
 * File:   Sound.cpp
 * Author: morgan
 * 
 * Created on May 6, 2014, 9:38 PM
 */

#include "Sound.h"

namespace mo {

    Sound::~Sound() {
    }
    
    unsigned int Sound::id() const {
        return id_;
    }
    
    void Sound::invalidate() {
        valid_ = false;
    }

    const bool Sound::valid() const {
        return valid_;
    }
    
    const char* Sound::data() const {
        return samples_.data();        
    }


}