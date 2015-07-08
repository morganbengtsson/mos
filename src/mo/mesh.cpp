/* 
 * File:   Model.cpp
 * Author: morgan
 * 
 * Created on February 25, 2014, 6:40 PM
 */

#include "mesh.hpp"

namespace mo {
    
    unsigned int Mesh::current_id = 0;
    
    Mesh::Mesh(std::initializer_list<Vertex> vertices, std::initializer_list<int> elements) : Mesh(vertices.begin(), vertices.end(), elements.begin(), elements.end()) {
    }

    Mesh::Mesh() : valid(true){
        id_ = current_id++;        
    }
	
    Mesh::Mesh(const Mesh & mesh): Mesh(mesh.vertices_begin(), mesh.vertices_end(), mesh.elements_begin(), mesh.elements_end()){		
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
        valid = false;
    }
    
    void Mesh::add(const int element){
        elements_.push_back(element);
    }

    const Mesh::Vertices & Mesh::vertices() {
        return vertices_;
    }

    const Mesh::Elements & Mesh::elements() {
        return elements_;
    }

    const Vertex * Mesh::vertices_data() const {
        return vertices_.data();
    }

    Mesh::Vertices::size_type Mesh::vertices_size() const {
        return vertices_.size();
    }

    const int * Mesh::elements_data() const {
        return elements_.data();
    }

    Mesh::Vertices::size_type Mesh::elements_size() const {
        return elements_.size();
    }

}
