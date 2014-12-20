/* 
 * File:   Sound.cpp
 * Author: morgan
 * 
 * Created on May 6, 2014, 9:38 PM
 */

#include "sound.hpp"

namespace mo {

    Sound::~Sound() {
    }
    
    unsigned int Sound::id() const {
        return id_;
    }
       
    const short * Sound::data() const {
        return samples_.data();        
    }


}
