/* 
 * File:   Source.cpp
 * Author: morgan
 * 
 * Created on July 28, 2014, 3:47 PM
 */

#include "source.hpp"

namespace mo {

    Source::~Source() {
    }
    
    const Source::Sounds::const_iterator Source::begin() const{
        return sounds_.begin();
    }
    const Source::Sounds::const_iterator Source::end() const{
        return sounds_.end();
    }

    unsigned int Source::id() const{
        return id_;
    }
}
