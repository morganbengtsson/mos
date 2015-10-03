/* 
 * File:   Sound.cpp
 * Author: morgan
 * 
 * Created on May 6, 2014, 9:38 PM
 */

#include "audio/sound.hpp"

namespace mos {
    Sound::~Sound() {
    }
    
    unsigned int Sound::id() const {
        return id_;
    }

    unsigned int Sound::channels() const {
        return channels_;
    }

    const short * Sound::data() const {
        return samples_.data();        
    }
}
