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

    unsigned int Texture2D::width() const {
        return width_;
    }

    unsigned int Texture2D::height() const {
        return height_;
    }
    
    glm::vec4 Texture2D::sample(const unsigned int x, const unsigned int y){
        unsigned int index = x*4 * height() + y*4;
        unsigned char r = texels_[index];
        unsigned char g = texels_[index + 1];
        unsigned char b = texels_[index + 2];
        unsigned char a = texels_[index + 3];
        return glm::vec4((float)r / 255.0f, 
                         (float)g / 255.0f, 
                         (float)b / 255.0f, 
                         (float)a / 255.0f);
    }
        
}