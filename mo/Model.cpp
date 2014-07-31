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
    transform(transform),
    valid_(true){
    }

    Model::~Model() {
    }
   
}