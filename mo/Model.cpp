/* 
 * File:   Model.cpp
 * Author: morgan
 * 
 * Created on March 5, 2014, 10:29 PM
 */

#include "Model.h"

namespace mo {

    Model::Model(Mesh * mesh, Texture2D * texture,
            const glm::mat4 transform)
    : mesh_(mesh), texture_(texture), transform_(transform), valid_(true) {
    }

    Mesh * Model::mesh() const {
        return mesh_.get();
    }

    Texture2D * Model::texture() const {
        return texture_.get();
    }

    Model::~Model() {
    }

    const glm::mat4 Model::transform() const {
        return transform_;
    }

}