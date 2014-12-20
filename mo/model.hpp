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
#include "Material.h"
#include "box.hpp"

namespace mo {

    /*!
     * A class that gathers what is needed to render an object. A Mesh, a texture
     * and a transformation (position/rotation).
     */
    class Model {
    public:
        enum class Draw {TRIANGLES, LINES, POINTS};
        Model();
        Model(std::shared_ptr<Mesh> mesh, 
              std::shared_ptr<Texture2D> texture,              
              const glm::mat4 transform = glm::mat4(1.0f),
              const Draw draw = Draw::TRIANGLES,
              std::shared_ptr<Material> material = 
                    std::make_shared<Material>(Material(glm::vec3(0.0f), 
                                                        glm::vec3(1.0f), 
                                                        glm::vec3(1.0f))),
              std::shared_ptr<Texture2D> lightmap = std::shared_ptr<Texture2D>());
        virtual ~Model();

        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Texture2D> texture;
        std::shared_ptr<Texture2D> lightmap;
        std::shared_ptr<Material> material;
        Box box;
        Draw draw;
        glm::mat4 transform;
        glm::vec4 color() const;
        glm::vec3 position() const;
        bool intersect(const glm::vec3 & origin, const glm::vec3 & direction);
        bool selected;
    private:
        bool valid_;
        

    };
}

#endif	/* MO_MODEL_H */

