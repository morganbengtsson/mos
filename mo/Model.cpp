/* 
 * File:   Model.cpp
 * Author: morgan
 * 
 * Created on March 5, 2014, 10:29 PM
 */

#include "Model.h"

namespace mo {

    Model::Model(std::shared_ptr<Assets> assets,
            const std::string mesh_path,
            const std::string texture_path,
            const glm::mat4 transform) :
    mesh_path_(mesh_path),
    texture_path_(texture_path),
    transform_(transform),
    valid_(true),
    assets_(assets) {
    }

    const Mesh & Model::mesh() const {
        return assets_->mesh(mesh_path_);
    }

    const Texture2D & Model::texture() const{
        return assets_->texture(texture_path_);
    }

    Model::~Model() {
    }

    const glm::mat4 Model::transform() const {
        return transform_;
    }

    void Model::setTransform(const glm::mat4 transform) {
        transform_ = transform;
    }

}