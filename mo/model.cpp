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
             std::shared_ptr<Texture2D> lightmap) :
    draw(draw),
    mesh(mesh),
    texture(texture),
    material(material),
    lightmap(lightmap),
    transform(transform),
    valid_(true), selected(false){

    glm::vec3 min, max;

    if (mesh->verticesBegin() != mesh->verticesEnd()){
        auto x_extremes = std::minmax_element(mesh->verticesBegin(), mesh->verticesEnd(),
                                              [](const Vertex& left, const Vertex& right) {
            return left.position.x < right.position.x;
        });

        auto y_extremes = std::minmax_element(mesh->verticesBegin(), mesh->verticesEnd(),
                                              [](const Vertex& left, const Vertex& right) {
            return left.position.y < right.position.y;
        });

        auto z_extremes = std::minmax_element(mesh->verticesBegin(), mesh->verticesEnd(),
                                              [](const Vertex& left, const Vertex& right) {
            return left.position.z < right.position.z;
        });

        min = glm::vec3(x_extremes.first->position.x, y_extremes.first->position.y, z_extremes.first->position.z);
        max = glm::vec3(x_extremes.second->position.x, y_extremes.second->position.y, z_extremes.second->position.z);
    }
    box = Box(min, max, transform);
}

Model::~Model() {
}

glm::vec4 Model::color() const {
    return glm::vec4(material->diffuse.r, material->diffuse.g, material->diffuse.b, material->opacity);
}

glm::vec3 Model::position() const {
    return (glm::vec3)(transform*glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

bool Model::intersect(const glm::vec3 & origin, const glm::vec3 & direction) {
    Ray ray(origin, direction);
    return box.intersect(ray, 0.0f, 100.0f);
}
std::pair<bool, glm::vec3> Model::intersect2(const glm::vec3 & p1, const glm::vec3 & p2){
    glm::vec3 hit;
    bool is_hit = box.intersect(p1, p2, hit);
    return std::pair<bool, glm::vec3>(is_hit, hit);
}

}
