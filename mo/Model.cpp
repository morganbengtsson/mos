/* 
 * File:   Model.cpp
 * Author: morgan
 * 
 * Created on March 5, 2014, 10:29 PM
 */

#include "Model.h"

namespace mo {

    Model::Model(std::shared_ptr<Mesh> mesh, 
            std::shared_ptr<Texture2D> texture,
            const glm::mat4 transform) :
    mesh(mesh),
    texture(texture),
    transform_(transform),
    valid_(true){
    }

    Model::~Model() {
    }

    const glm::mat4 Model::transform() const {
        return transform_;
    }

    void Model::setTransform(const glm::mat4 transform) {
        transform_ = transform;
    }

    const bool Model::valid() const {
        return valid_;
    }
    void Model::invalidate() {
        valid_ = false;
    }
}