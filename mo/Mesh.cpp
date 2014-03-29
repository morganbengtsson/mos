/* 
 * File:   Model.cpp
 * Author: morgan
 * 
 * Created on February 25, 2014, 6:40 PM
 */

#include "Mesh.h"

namespace mo {

    Mesh::~Mesh() {
    }

    const unsigned int Mesh::id() const {
        return id_;
    }

}