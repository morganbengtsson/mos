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

    class Model {
    public:
        Model(std::shared_ptr<Assets> assets, const std::string mesh_path, const std::string texture_path, const glm::mat4 transform = glm::mat4(1.0f));
        virtual ~Model();

        const Mesh & mesh() const ;
        const Texture2D & texture() const;
        
        const glm::mat4 transform() const;
        void setTransform(const glm::mat4 transform);
        const bool valid() const;
        void invalidate();
    private:
        bool valid_;
        glm::mat4 transform_;
        std::shared_ptr<Assets> assets_;
        const std::string mesh_path_;
        const std::string texture_path_;
    };
}

#endif	/* MO_MODEL_H */

