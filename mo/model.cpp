/* 
 * File:   Model.cpp
 * Author: morgan
 *
 * Created on March 5, 2014, 10:29 PM
 */

#include "model.hpp"

#include <glm/gtx/io.hpp>
#include <iostream>
namespace mo {

Model::Model(){

}
Model::Model(std::shared_ptr<Mesh> mesh,
             std::shared_ptr<Texture2D> texture,
             const glm::mat4 transform,
             const Draw draw,
             std::shared_ptr<Material> material,
             std::shared_ptr<Texture2D> lightmap,
             std::shared_ptr<Texture2D> normalmap) :
    draw(draw),
    mesh(mesh),
    texture(texture),
    material(material),
    lightmap(lightmap),
    normalmap(normalmap),
    transform(transform),
    valid_(true),
    selected_(false),
    selectable(true),
    box(mesh->vertices_end(), mesh->vertices_end(), transform){}

Model::~Model() {
}

glm::vec4 Model::color() const {
    return glm::vec4(material->diffuse.r, material->diffuse.g, material->diffuse.b, material->opacity);
}

glm::vec3 Model::position() const {
    return (glm::vec3)(transform*glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

bool Model::selected() const {
    return selected_;
}

void Model::selected(const bool selected) {
    selected_ = selectable == true ? selected : false;
}

}
