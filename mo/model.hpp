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
#include <utility>

#include "mesh.hpp"
#include "texture2d.hpp"
#include "assets.hpp"
#include "material.hpp"
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
        glm::mat4 transform;
        Box box;
        Draw draw;
        glm::vec4 color() const;
        glm::vec3 position() const;
        std::pair<bool, glm::vec3> intersect(const glm::vec3 & origin, const glm::vec3 direction, float t1, float t2);
        std::pair<bool, glm::vec3> intersect(const glm::vec3 & point1, const glm::vec3 & point2);
        bool selected;
    private:
        bool valid_;
        

    };
}

#endif	/* MO_MODEL_H */

