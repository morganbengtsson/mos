/* 
 * File:   Model.cpp
 * Author: morgan
 * 
 * Created on February 25, 2014, 6:40 PM
 */

#include "graphics/mesh.hpp"
#include "util.hpp"

namespace mos {
    
    unsigned int Mesh::current_id = 0;
    
    Mesh::Mesh(std::initializer_list<Vertex> vertices, std::initializer_list<int> elements) :
        Mesh(vertices.begin(), vertices.end(), elements.begin(), elements.end()) {
    }

    Mesh::Mesh() : valid_(false){
        id_ = current_id++;        
    }
	
    Mesh::Mesh(const Mesh & mesh): Mesh(mesh.vertices_begin(),
                                        mesh.vertices_end(),
                                        mesh.elements_begin(),
                                        mesh.elements_end()){
	}		
    
    Mesh::~Mesh() {
    }

    Mesh::Vertices::const_iterator Mesh::vertices_begin() const {
        return vertices_.begin();
    }

    Mesh::Vertices::const_iterator Mesh::vertices_end() const {
        return vertices_.end();
    }

    Mesh::Elements::const_iterator Mesh::elements_begin() const {
        return elements_.begin();
    }

    Mesh::Elements::const_iterator Mesh::elements_end() const {
        return elements_.end();
    }

    Mesh::Vertices::iterator Mesh::vertices_begin() {
        return vertices_.begin();
    }

    Mesh::Vertices::iterator Mesh::vertices_end() {
        return vertices_.end();
    }

    Mesh::Elements::iterator Mesh::elements_begin() {
        return elements_.begin();
    }

    Mesh::Elements::iterator Mesh::elements_end() {
        return elements_.end();
    }

    unsigned int Mesh::id() const {
        return id_;
    }

    bool Mesh::valid() const {
        return valid_;
    }

    void Mesh::invalidate() {
        valid_ = false;
    }

    void Mesh::clear() {
        vertices_.clear();
        elements_.clear();
    }
    
    void Mesh::add(const Vertex vertex) {
        vertices_.push_back(vertex);
        valid_ = false;
    }
    
    void Mesh::add(const int element){
        elements_.push_back(element);
        valid_ = false;
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

    void Mesh::mix(const Mesh & mesh1,
                   const Mesh & mesh2,
                   const float amount) {
        auto it = vertices_begin();
        auto it1 = mesh1.vertices_begin();
        auto it2 = mesh2.vertices_begin();

        while(it != vertices_end()) {
            it->position = glm::mix(it1->position, it2->position, amount);
            it->normal = glm::mix(it1->normal, it2->normal, amount);
            it->uv = glm::mix(it1->uv, it2->uv, amount);
            it->uv_lightmap = glm::mix(it1->uv_lightmap, it2->uv_lightmap, amount);

            it++;
            it1++;
            it2++;
        }
        invalidate();
    }



}
