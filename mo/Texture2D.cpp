/* 
 * File:   Texture.cpp
 * Author: morgan
 * 
 * Created on February 25, 2014, 7:26 PM
 */

#include "Texture2D.h"

namespace mo{

    Texture2D::~Texture2D() {
    }

    const unsigned int Texture2D::id() const {
        return id_;
    }

    unsigned int Texture2D::width(){
        return width_;
    }

    unsigned int Texture2D::height(){
        return height_;
    }
}