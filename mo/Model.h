/* 
 * File:   Model.h
 * Author: Morgan Bengtsson, bengtsson.morgan@gmail.com
 *
 * Created on March 5, 2014, 10:29 PM
 */

#ifndef MO_MODEL_H
#define	MO_MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <memory>

#include "Mesh.h"
#include "Texture2D.h"
#include "Assets.h"

namespace mo {

    /*!
     * A class that gathers what is needed to render an object. A Mesh, a texture
     * and a transformation (position/rotation).
     */
    class Model {
    public:
        Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture2D> texture, const glm::mat4 transform = glm::mat4(1.0f));
        virtual ~Model();

        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Texture2D> texture;

        glm::mat4 transform;
        glm::vec3 position();
        
    private:
        bool valid_;
        

    };
}

#endif	/* MO_MODEL_H */

