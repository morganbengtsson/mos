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
     * A class that gathers what is needed to render an object. A Mesh, a Texture2D
     * , a Texture2D lightmap, a Texture2D normalmap and a Material.
     * Also a transformation (position/rotation).
     */
    class Model {
    public:
        enum class Draw {TRIANGLES, LINES, POINTS};
        /*!
         * \brief Model
         */
        Model();

        /*!
         * \brief Model
         * \param mesh
         * \param texture
         * \param transform
         * \param draw
         * \param material
         * \param lightmap
         * \param normalmap
         */
        Model(std::shared_ptr<Mesh> mesh, 
              std::shared_ptr<Texture2D> texture,              
              const glm::mat4 transform = glm::mat4(1.0f),
              const Draw draw = Draw::TRIANGLES,
              std::shared_ptr<Material> material = 
                    std::make_shared<Material>(Material(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f))),
              std::shared_ptr<Texture2D> lightmap = std::shared_ptr<Texture2D>(),
              std::shared_ptr<Texture2D> normalmap = std::shared_ptr<Texture2D>(),
              const bool selectable = false);
        virtual ~Model();

        /*!
         * \brief mesh
         */
        std::shared_ptr<Mesh> mesh;
        /*!
         * \brief texture
         */
        std::shared_ptr<Texture2D> texture;
        /*!
         * \brief lightmap
         */
        std::shared_ptr<Texture2D> lightmap;
        /*!
         * \brief normalmap
         */
        std::shared_ptr<Texture2D> normalmap;
        /*!
         * \brief material
         */
        std::shared_ptr<Material> material;
        /*!
         * \brief transform
         * \return
         */

        glm::mat4 transform() const;
        /*!
         * \brief transform
         * \param transform
         */
        void transform(const glm::mat4 & transform);

        glm::vec3 position() const;

        void position(const glm::vec3 position);

        glm::vec4 color() const;

        Box box;
        Draw draw;
        bool selectable;
        bool selected() const;
        void selected(const bool selected);
    private:
        bool valid_;
        bool selected_;
        glm::mat4 transform_;
    };
}

#endif	/* MO_MODEL_H */

