/* 
 * File:   Model.h
 * Author: morgan
 *
 * Created on March 5, 2014, 10:29 PM
 */

#ifndef OGLI_MODEL_H
#define	OGLI_MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <memory>

#include "Mesh.h"
#include "Texture2D.h"

namespace mo {

    class Model {
    public:
        Model(Mesh * mesh, Texture2D * texture, const glm::mat4 transform);
        virtual ~Model();

        Mesh * mesh() const;
        Texture2D * texture() const;

        const glm::mat4 transform() const;
        const bool valid() const;
        void invalidate();
    private:
        bool valid_;
        glm::mat4 transform_;
        std::shared_ptr<Mesh> mesh_;
        std::shared_ptr<Texture2D> texture_;
    };
}

#endif	/* OGLI_MODEL_H */

