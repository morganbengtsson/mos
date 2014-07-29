/* 
 * File:   Model.cpp
 * Author: morgan
 * 
 * Created on February 25, 2014, 6:40 PM
 */

#include "Mesh.h"

namespace mo {
    
    Mesh::Mesh() : valid(true){        
    }
    
    Mesh::~Mesh() {
    }

    unsigned int Mesh::id() const {
        return id_;
    }
    
    void Mesh::clear() {
        vertices_.clear();
        elements_.clear();
    }
    
    void Mesh::add(const Vertex vertex) {
        vertices_.push_back(vertex);
    }
    
    void Mesh::add(const int element){
        elements_.push_back(element);
    }
    
    Vertex Mesh::back(){
        return vertices_.back();
    }
    
}